/* ExwordLibrary - Thread class for installing and removing add-ons
 *
 * Copyright (C) 2011 - Brian Johnson <brijohn@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef INSTALLTHREAD_H
#define INSTALLTHREAD_H

#include "ExwordDevice.h"
#include "ThreadBase.h"


class InstallThread : public ThreadBase
{
    public:
        InstallThread(wxFrame *frame, Exword *exword) : ThreadBase(frame) {
            m_exword = exword;
        };
        virtual void *Action();
    private:
        Exword *m_exword;
};

class RemoveThread : public ThreadBase
{
    public:
        RemoveThread(wxFrame *frame, Exword *exword) : ThreadBase(frame) {
            m_exword = exword;
        };
        virtual void *Action();
    private:
        Exword *m_exword;
};

#endif

