#pragma once

#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include "VehicleVals.h"


class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	class wAbout : public wxFrame
	{
	public:
		wAbout(cMain* ref) : wxFrame(ref, 11000, "About", wxDefaultPosition, wxSize(280, 150), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) | wxFRAME_FLOAT_ON_PARENT)
		{
			Centre();
			SetBackgroundColour(wxColor(0xFF, 0xFF, 0xFF));
			text = new wxStaticText(this, wxID_ANY, "Vehicle Editing Tool for .tbl files, version 1.03\nProgrammed by MeToX\nCopyright (c) 2022", wxPoint(10, 10));
			okBtn = new wxButton(this, 11001, "OK", wxPoint(100, 75));
		}

		void onOk(wxCommandEvent& evt) {
			Close();
		}

		wxDECLARE_EVENT_TABLE();

	private:
		wxStaticText* text = nullptr;
		wxButton* okBtn = nullptr;
	};

public:
	wxButton* m_btn1 = nullptr;
	wxButton* m_btn2 = nullptr;

	wxTextCtrl* m_txt1 = nullptr;

	VehicleVals* vehicleVaues = nullptr;

	void OnMenuOpenFile(wxCommandEvent& evt);
	void OnMenuSaveFile(wxCommandEvent& evt);
	void OnMenuSaveFileAs(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);
	void OnExportTxt(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnValueChange(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);

	wxDECLARE_EVENT_TABLE();

private:

	wxMenuBar* m_MenuBar = nullptr;
	wxMenu* menuFile = nullptr;
	wxMenu* menuExtras = nullptr;

	wxPanel* topInfoPlane = nullptr;
	wxStaticText* infoText = nullptr;

	wxBoxSizer* vbp = nullptr;
	wxScrolledWindow* scrolleditemList = nullptr;

	std::vector<std::vector<wxTextCtrl*>>dataFields;

	wAbout* about = nullptr;

	bool dataChanged = false;

	wxString pathOpened;

	void refreshDisplayedData();
	bool updateFromFields();
};
