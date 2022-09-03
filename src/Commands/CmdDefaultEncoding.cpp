﻿// This file is part of BowPad.
//
// Copyright (C) 2014, 2016-2017, 2020-2022 - Stefan Kueng
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See <http://www.gnu.org/licenses/> for a copy of the full license text
//
#include "stdafx.h"
#include "CmdDefaultEncoding.h"
#include "BowPad.h"
#include "Theme.h"

LRESULT CDefaultEncodingDlg::DlgFunc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            InitDialog(hwndDlg, IDI_BOWPAD);
            CTheme::Instance().SetThemeForDialog(*this, CTheme::Instance().IsDarkTheme());

            UINT      cp         = static_cast<UINT>(CIniSettings::Instance().GetInt64(L"Defaults", L"encodingnew", GetACP()));
            bool      bom        = CIniSettings::Instance().GetInt64(L"Defaults", L"encodingnewbom", 0) != 0;
            bool      preferUtf8 = CIniSettings::Instance().GetInt64(L"Defaults", L"encodingutf8overansi", 0) != 0;
            EOLFormat eol        = static_cast<EOLFormat>(CIniSettings::Instance().GetInt64(L"Defaults", L"lineendingnew", static_cast<int>(EOLFormat::Win_Format)));

            if (cp == GetACP())
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_ANSI);
            else if (cp == CP_UTF8)
            {
                if (bom)
                    CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF8BOM);
                else
                    CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF8);
            }
            else if (cp == 1200)
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF16LE);
            else if (cp == 1201)
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF16BE);
            else if (cp == 12000)
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF32LE);
            else if (cp == 12001)
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_UTF32BE);
            else
                CheckRadioButton(*this, IDC_R_ANSI, IDC_R_UTF32BE, IDC_R_ANSI);

            CheckDlgButton(*this, IDC_LOADASUTF8, preferUtf8 ? BST_CHECKED : BST_UNCHECKED);

            switch (eol)
            {
                default:
                case EOLFormat::Win_Format:
                    CheckRadioButton(*this, IDC_CRLF_RADIO, IDC_CR_RADIO, IDC_CRLF_RADIO);
                    break;
                case EOLFormat::Mac_Format:
                    CheckRadioButton(*this, IDC_CRLF_RADIO, IDC_CR_RADIO, IDC_CR_RADIO);
                    break;
                case EOLFormat::Unix_Format:
                    CheckRadioButton(*this, IDC_CRLF_RADIO, IDC_CR_RADIO, IDC_LF_RADIO);
                    break;
            }
        }
            return FALSE;
        case WM_COMMAND:
            return DoCommand(LOWORD(wParam), HIWORD(wParam));
        default:
            return FALSE;
    }
}

LRESULT CDefaultEncodingDlg::DoCommand(int id, int /*msg*/)
{
    switch (id)
    {
        case IDCANCEL:
            EndDialog(*this, id);
            break;
        case IDOK:
        {
            UINT cp         = GetACP();
            bool bom        = false;
            bool preferUtf8 = IsDlgButtonChecked(*this, IDC_LOADASUTF8) == BST_CHECKED;

            if (IsDlgButtonChecked(*this, IDC_R_ANSI))
                cp = GetACP();
            else if (IsDlgButtonChecked(*this, IDC_R_UTF8))
                cp = CP_UTF8;
            else if (IsDlgButtonChecked(*this, IDC_R_UTF8BOM))
            {
                cp  = CP_UTF8;
                bom = true;
            }
            else if (IsDlgButtonChecked(*this, IDC_R_UTF16LE))
                cp = 1200;
            else if (IsDlgButtonChecked(*this, IDC_R_UTF16BE))
                cp = 1201;
            else if (IsDlgButtonChecked(*this, IDC_R_UTF32LE))
                cp = 12000;
            else if (IsDlgButtonChecked(*this, IDC_R_UTF32BE))
                cp = 12001;

            if (IsDlgButtonChecked(*this, IDC_CRLF_RADIO))
                CIniSettings::Instance().SetInt64(L"Defaults", L"lineendingnew", static_cast<int>(EOLFormat::Win_Format));
            if (IsDlgButtonChecked(*this, IDC_CR_RADIO))
                CIniSettings::Instance().SetInt64(L"Defaults", L"lineendingnew", static_cast<int>(EOLFormat::Mac_Format));
            if (IsDlgButtonChecked(*this, IDC_LF_RADIO))
                CIniSettings::Instance().SetInt64(L"Defaults", L"lineendingnew", static_cast<int>(EOLFormat::Unix_Format));

            CIniSettings::Instance().SetInt64(L"Defaults", L"encodingnew", cp);
            CIniSettings::Instance().SetInt64(L"Defaults", L"encodingnewbom", bom);
            CIniSettings::Instance().SetInt64(L"Defaults", L"encodingutf8overansi", preferUtf8);

            EndDialog(*this, id);
        }
        [[fallthrough]];
        default:
            break;
    }
    return 1;
}

bool CCmdDefaultEncoding::Execute()
{
    CDefaultEncodingDlg dlg;
    dlg.DoModal(g_hRes, IDD_DEFAULTENCODING, GetHwnd());

    return true;
}
