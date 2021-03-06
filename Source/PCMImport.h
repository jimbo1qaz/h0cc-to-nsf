/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#pragma once

#include "stdafx.h"		// // //
#include "../resource.h"		// // //
#include <memory>		// // //
#include "PCMImporter.h"		// // //

class CPCMImport : public CDialog
{
	DECLARE_DYNAMIC(CPCMImport)

public:
	CPCMImport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPCMImport();

// Dialog Data
	enum { IDD = IDD_PCMIMPORT };

	std::shared_ptr<ft0cc::doc::dpcm_sample> ShowDialog();		// // //

protected:
	std::shared_ptr<ft0cc::doc::dpcm_sample> m_pImported;		// // //
	std::shared_ptr<ft0cc::doc::dpcm_sample> m_pCachedSample;

	CStringW m_strPath;
	CStringW m_strFileName;

	CPCMImporter m_Importer;		// // //

	int m_iQuality = QUALITY_RANGE - 1;
	int m_iVolume = 0;
	int m_iCachedQuality = 0;
	int m_iCachedVolume = 0;

protected:
	static const int QUALITY_RANGE;
	static const int VOLUME_RANGE;

protected:
	std::shared_ptr<ft0cc::doc::dpcm_sample> GetSample();		// // //
	std::shared_ptr<ft0cc::doc::dpcm_sample> ConvertFile();

	bool OpenWaveFile();
	void UpdateFileInfo();
	void UpdateText();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedPreview();
};
