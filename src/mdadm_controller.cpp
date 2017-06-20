/*
 * Class responsible for communication with mdadm process
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mdadm_controller.hpp"

#include <QRegExp>
#include <QFile>
#include <QTextStream>

#include "imdadm_process.hpp"

namespace
{
    QString levelName(MDAdmController::Type type)
    {
        QString result;

        switch(type)
        {
            case MDAdmController::Type::Raid0:
                result = "stripe";
                break;

            case MDAdmController::Type::Raid1:
                result = "mirror";
                break;

            case MDAdmController::Type::Raid4:
                result = "4";
                break;

            case MDAdmController::Type::Raid5:
                result = "5";
                break;

            case MDAdmController::Type::Raid6:
                result = "6";
                break;
        }

        return result;
    }
}


MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess): m_mdadmProcess(mdadmProcess)
{

}


MDAdmController::~MDAdmController()
{

}


bool MDAdmController::listRaids (const ListResult& result)
{
    QFile mdstat("/proc/mdstat");

    const bool open_status = mdstat.exists() && mdstat.open(QIODevice::ReadOnly |
                                                            QIODevice::Text);

    if (open_status)
    {
        //                        raid device  status   type   devices
        const QRegExp mdadm_info("^(md[^ ]+) : ([^ ]+) ([^ ]+) (.*)");
        std::vector<RaidInfo> results;

        // simple loop with atEnd() won't work
        // see: http://doc.qt.io/qt-5/qiodevice.html#atEnd
        // and  http://doc.qt.io/qt-5/qfile.html#details
        // for details
        QTextStream mdstat_stream(&mdstat);

        for(QString outputLine = mdstat_stream.readLine(); outputLine.isNull() == false; outputLine = mdstat_stream.readLine())
            if (mdadm_info.exactMatch(outputLine))
            {
                const QString dev = mdadm_info.cap(1);
                const QString status = mdadm_info.cap(2);
                const QString type = mdadm_info.cap(3);
                const QString devices = mdadm_info.cap(4);

                const QStringList devices_list_raw = devices.split(" ");

                //drop role numbers (convert sda1[0] to sda1)
                QStringList devices_list;
                for(const QString& device_with_role: devices_list_raw)
                {
                    const int l = device_with_role.lastIndexOf('[');
                    const QString device = device_with_role.left(l);

                    devices_list.append(device);
                }

                results.emplace_back(dev, devices_list, type);
            }

        mdstat.close();

        result(results);
    }

    return open_status;
}


bool MDAdmController::createRaid(const QString& raid_device, MDAdmController::Type type, const QStringList& block_devices)
{
    QStringList mdadm_args;

    mdadm_args << "--create" << "--verbose" << raid_device;
    mdadm_args << "--level" << levelName(type);
    mdadm_args << "--raid-devices=" + block_devices.size() << block_devices;

    m_mdadmProcess->execute(mdadm_args, [](const QByteArray& output)
    {
    });

    return true;
}
