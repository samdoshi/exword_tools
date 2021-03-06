/* ExwordTools - Classes representing remote and local dictionaries
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

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/regex.h>
#include <wx/longlong.h>

#include "Dictionary.h"
#include "ExwordDevice.h"

char valid_sfn_chars[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01,
};

DictionaryInfo InvalidDictionary("", "", "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");

bool DictionaryInfo::operator==(const DictionaryInfo& other)
{
    return (this->m_id == other.m_id && this->m_key == other.m_key && this->m_name == other.m_name);
}

DictionaryInfo::DictionaryInfo(const char *id, const char *name, const char *key)
{
    m_id = wxString::FromAscii(id);
    m_name = wxString(name, wxCSConv(wxT("SJIS")));
    m_key.AppendData(key, 16);
}

Dictionary::Dictionary(wxString id)
{
    m_id = id;
}

wxString Dictionary::GetId()
{
    return m_id;
}

LocalDictionary::LocalDictionary(wxString id, ExwordRegion region) : Dictionary(id)
{
    static const wxChar *country[] = { NULL, wxT("ja"), wxT("cn"),
                                       wxT("kr"), wxT("de"), wxT("es"),
                                       wxT("fr"), wxT("ru")};
    m_path.Printf(wxT("%s%c%s%c%s%c"), Exword::GetUserDataDir().c_str(),
                  wxFileName::GetPathSeparator(), country[region],
                  wxFileName::GetPathSeparator(), id.c_str(),
                  wxFileName::GetPathSeparator());
}

wxString LocalDictionary::GetName()
{
    wxString name = wxT("");
    wxFile diction(m_path + wxT("diction.htm"));
    char *html, *start, *end;
    int len;
    if (diction.IsOpened()) {
        html = new char[diction.Length()];
        if (diction.Read(html, diction.Length()) != wxInvalidOffset) {
            start = strstr(html, "<title>");
	    end = strstr(html, "</title>");
            len = end - (start + 7);
            if (start != NULL && end != NULL && len > 0) {
                name = wxString(start + 7, wxCSConv(wxT("SJIS")), len);
            }
        }
        delete [] html;
    }
    return name;
}

unsigned long long LocalDictionary::GetSize()
{
    wxULongLong size = wxDir::GetTotalSize(m_path);
    return (size == wxInvalidSize ? 0 : size.GetValue());
}

bool LocalDictionary::Exists()
{
    wxString dictionFile = m_path + wxT("diction.htm");
    return wxFileName::FileExists(dictionFile);
}

wxArrayString LocalDictionary::GetFiles()
{
    wxArrayString files;
    wxString filename;
    wxDir dir(m_path);
    if (dir.IsOpened()) {
        bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
        while (cont) {
            if (IsValidSFN(filename))
               files.Add(m_path + filename);
            cont = dir.GetNext(&filename);
        }
    }
    return files;
}

unsigned long LocalDictionary::GetMinSupportedSeries()
{
    wxString filename;
    unsigned long min_series = 99;
    wxDir dir(m_path);
    if (dir.IsOpened()) {
        unsigned long val;
        bool cont = dir.GetFirst(&filename, wxT("infodp?.htm"), wxDIR_FILES);
        while (cont) {
            if (filename.Mid(6, 1).ToULong(&val)) {
                if (val != 0 && val < min_series)
                    min_series = val;
            }
            cont = dir.GetNext(&filename);
        }
    }
    return min_series;
}

bool LocalDictionary::IsValidSFN(wxString filename)
{
    size_t nameLen = 0;
    size_t extLen = 0;
    bool valid = false;
    unsigned char *name = (unsigned char *)strdup(filename.utf8_str().data());
    unsigned char *ext = (unsigned char *)strrchr((char *)name, '.');
    if (ext != NULL) {
        *ext++ = '\0';
        extLen = strlen((char *)ext);
    }
    nameLen = strlen((char *)name);
    printf("Name: %s Ext: %s\n", name, ext);
    if (nameLen > 8 || nameLen == 0 || extLen > 3)
        goto done;
    for (int i = 0; i < nameLen; ++i) {
        if (valid_sfn_chars[(unsigned char)name[i]] == 0)
            goto done;
    }
    for (int i = 0; i < extLen; ++i) {
        if (valid_sfn_chars[(unsigned char)ext[i]] == 0)
            goto done;
    }
    valid = true;
done:
    free(name);
    return valid;
}

RemoteDictionary::RemoteDictionary(wxString id, Exword *device) : Dictionary(id)
{
    m_device = device;
    DictionaryInfo info = device->GetDictionaryInfo(id);
    m_name = info.GetName();
    m_path = id + wxT("\\_CONTENT");
}

wxString RemoteDictionary::GetName()
{
    return m_name;
}

unsigned long long RemoteDictionary::GetSize()
{
    return 0;
}

bool RemoteDictionary::Exists()
{
    return (m_device->GetDictionaryInfo(m_id) == InvalidDictionary ? false : true);
}

wxArrayString RemoteDictionary::GetFiles()
{
    wxArrayString fileList;
    DirEnts files = m_device->List(m_path);
    for (int i = 0; i < files.GetCount(); ++i) {
        fileList.Add(files[i].GetFilename());
    }
    return fileList;
}

unsigned long RemoteDictionary::GetMinSupportedSeries()
{
    return 0;
}

