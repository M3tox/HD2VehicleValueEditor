#include "cMain.h"

#define OPENFILE		10001
#define SAVEFILE		10002
#define SAVEFILEAS		10003
#define EXPORTTXT		10004
#define ABOUT			10005
#define EXITAPP			10006
#define VALUECHNG		10007

wxBEGIN_EVENT_TABLE(cMain::wAbout, wxFrame)
EVT_BUTTON(11001, cMain::wAbout::onOk)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_MENU(OPENFILE, cMain::OnMenuOpenFile)
EVT_MENU(SAVEFILE, cMain::OnMenuSaveFile)
EVT_MENU(SAVEFILEAS, cMain::OnMenuSaveFileAs)
EVT_MENU(EXPORTTXT, cMain::OnExportTxt)
EVT_MENU(ABOUT, cMain::OnAbout)
EVT_MENU(EXITAPP, cMain::OnMenuExit)
EVT_TEXT(VALUECHNG, cMain::OnValueChange)
EVT_CLOSE(cMain::OnClose)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "H&D2 Vehicle Value Editor", wxDefaultPosition, wxSize(800, 600)) {

	Centre();
	// add menu bar
	m_MenuBar = new wxMenuBar();
	this->SetMenuBar(m_MenuBar);
	// add items
	menuFile = new wxMenu();
	menuFile->Append(OPENFILE, "Open File");
	menuFile->Append(SAVEFILE, "Save")->Enable(false);
	menuFile->Append(SAVEFILEAS, "Save as...")->Enable(false);
	menuFile->AppendSeparator();
	menuFile->Append(EXITAPP, "Exit");
	m_MenuBar->Append(menuFile, "File");

	// TODO
	menuExtras  = new wxMenu();
	menuExtras->Append(EXPORTTXT, "Export as text file")->Enable(false);
	menuExtras->Append(ABOUT, "About...");
	m_MenuBar->Append(menuExtras,"Extras");

	topInfoPlane = new wxPanel(this, wxID_ANY);
	topInfoPlane->SetBackgroundColour(wxColor(210, 210, 210));

	infoText = new wxStaticText(topInfoPlane, wxID_ANY, "No file loaded",wxDefaultPosition,wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	wxBoxSizer* hbp = new wxBoxSizer(wxHORIZONTAL);
	hbp->Add(infoText, 1, wxALIGN_CENTRE);
	topInfoPlane->SetSizerAndFit(hbp);

	vbp = new wxBoxSizer(wxVERTICAL);

	vbp->Add(topInfoPlane,0, wxGROW);
	//frame->SetSizer(vbp);

	//frame->Show();

	//this->SetSizerAndFit(vbp);
	this->SetSizer(vbp);
	
}

cMain::~cMain() {

}

void cMain::OnMenuOpenFile(wxCommandEvent& evt) {
	if (dataChanged)
	{
		if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("WARNING"),
			wxICON_QUESTION | wxYES_NO, this) == wxNO)
			return;
		//else: proceed asking to the user the new file to open
	}

	wxFileDialog openFileDialog(this, _("Open Vehicle Configuration"), "", "", "Table files (*.tbl)|*.tbl", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	// proceed loading the file chosen by the user;
	// this can be done with e.g. wxWidgets input streams:
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	if (openFileDialog.GetPath().empty()) return;

	if (vehicleVaues)
		delete vehicleVaues;

	pathOpened.assign(openFileDialog.GetPath());
	vehicleVaues = new VehicleVals(pathOpened.ToStdString());

	if (!vehicleVaues->checkError().empty()) {
		wxMessageBox(vehicleVaues->checkError(), "Error Message", wxICON_WARNING | wxOK);

		delete vehicleVaues;
		vehicleVaues = nullptr;
		return;
	}

	
	menuFile->FindItemByPosition(1)->Enable();
	menuFile->FindItemByPosition(2)->Enable();
	menuExtras->FindItemByPosition(0)->Enable();

	infoText->SetLabelText(openFileDialog.GetFilename());
	refreshDisplayedData();

	evt.Skip();
}

void cMain::OnMenuSaveFile(wxCommandEvent& evt) {
	if (!updateFromFields()) return;

	vehicleVaues->writeBinaryFile(pathOpened.ToStdString());

	dataChanged = false;
	evt.Skip();
}

void cMain::OnMenuSaveFileAs(wxCommandEvent& evt) {
	if (!updateFromFields()) return;

	wxFileDialog saveFileDialog(this, _("Save Table file"), "", "", "TBL files (*.tbl)|*.tbl", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	vehicleVaues->writeBinaryFile(saveFileDialog.GetPath().ToStdString());
	infoText->SetLabelText(saveFileDialog.GetFilename());
	dataChanged = false;
	evt.Skip();
}

void cMain::OnExportTxt(wxCommandEvent& evt) {
	if (!updateFromFields()) return;

	wxFileDialog saveFileDialog(this, _("Save Text file"), "", "", "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...


	vehicleVaues->writeTextFile(saveFileDialog.GetPath().ToStdString());
	evt.Skip();
}

void cMain::OnMenuExit(wxCommandEvent& evt) {
	/*
	if (dataChanged)
	{
		if (wxMessageBox(_("Current content has not been saved! Exit anyway?"), _("WARNING"),
			wxICON_QUESTION | wxYES_NO, this) == wxNO)
			return;
		//else: proceed asking to the user the new file to open
	}
	*/
	Close();
	evt.Skip();
}

void cMain::OnClose(wxCloseEvent& evt)
{
	if (evt.CanVeto() && dataChanged)
	{
		if (wxMessageBox("Current content has not been saved! Exit anyway?",
			"WARNING", wxICON_QUESTION | wxYES_NO) != wxYES)
		{
			evt.Veto();
			return;
		}
	}
	Destroy();
	evt.Veto();
}

void cMain::refreshDisplayedData() {

	uint32_t id;
	std::string descr;
	std::string val;

	//delete vbp;
	
	//vbp->SetMinSize(800, 600);

	//wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	//buttonSizer->AddSpacer(512);
	//vbp->Add(buttonSizer);

	//wxFrame* frame = new wxFrame((wxFrame*)NULL, -1, wxT("Scrolling Widgets"), wxPoint(50, 50), wxSize(650, 650));
	if (scrolleditemList) {
		// IF its the exact same data structure, recycle the current one and only update the data fields!
		
		// TODO: if something different was loaded already, delete the old content
		// in theory we could keep the cells and descriptions, just updating the fields
		// but I may want it to load different data in the future
		//vbp->Detach(scrolleditemList);
		//scrolleditemList->Destroy();

		// update only data fields
		const uint32_t sectionCount = dataFields.size();
		for (uint32_t t = 0; t < sectionCount; t++) {

			const uint32_t itemCount = vehicleVaues->getSectionItemCount(t);
			for (uint32_t i = 0; i < itemCount; i++) {
				if (!vehicleVaues->getItemData(t, i, id, descr, val)) {
					wxMessageBox("ERROR: Item index out of bounds!", "Error Message", wxICON_ERROR | wxOK);
					break;
				}
				if (!vehicleVaues->checkIfDescrOnly(t, i)) {
					dataFields.at(t).at(id-1)->SetValue(val);
				}
			}
		}
		vbp->Layout();
		dataChanged = false;
		return;
	}


	scrolleditemList = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	scrolleditemList->Hide();

	vbp->Add(scrolleditemList, 1, wxEXPAND);


	wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

	// add a series of widgets
	const uint32_t sectionCount = vehicleVaues->getTotalSectionCount();
	dataFields.resize(sectionCount);
	for (uint32_t t = 0; t < sectionCount; t++) {

		itemSizer->AddSpacer(15);
		wxStaticText* m_sectionTitle  = new wxStaticText(scrolleditemList, wxID_ANY, vehicleVaues->getSectionTitle(t));
		itemSizer->Add(m_sectionTitle, 1, wxGROW | wxALL, 2);
		
		wxFont font = m_sectionTitle->GetFont();
		font.SetPointSize(10);
		font.SetWeight(wxFONTWEIGHT_BOLD);
		m_sectionTitle->SetFont(font);

		const uint32_t itemCount = vehicleVaues->getSectionItemCount(t);
		dataFields.at(t).reserve(itemCount);
		for (uint32_t i = 0; i < itemCount; i++) {

			wxBoxSizer* colSizer = new wxBoxSizer(wxHORIZONTAL);
			colSizer->AddSpacer(10);

			if (!vehicleVaues->getItemData(t, i, id, descr, val)) {
				wxMessageBox("Item index out of bounds!", "Error Message", wxICON_ERROR | wxOK);
				break;
			}

			if (!vehicleVaues->checkIfDescrOnly(t, i)) {
				colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, std::to_string(id)), 1, wxGROW | wxALL, 2);
				colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, descr), 3, wxGROW | wxALL, 2);

				dataFields.at(t).emplace_back(new wxTextCtrl(scrolleditemList, VALUECHNG, val));
				colSizer->Add(dataFields.at(t).back(), 3, wxGROW | wxALL, 2);
			}
			else {
				colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, descr), 3, wxGROW | wxALL, 2);
			}


			itemSizer->Add(colSizer, 1, wxEXPAND);
		}
	}
	

	scrolleditemList->SetSizer(itemSizer);

	// this part makes the scrollbars show up
	//my_image->FitInside(); // ask the sizer about the needed size
	scrolleditemList->SetScrollRate(20, 20);
	dataChanged = false;
	//vbp->Add(my_image);
	//my_image->Refresh();
	scrolleditemList->Show();
	vbp->Layout();
	
}

bool cMain::updateFromFields() {
	// take data from input fields and update internal data
	const uint32_t sectionCount = dataFields.size();
	for (uint32_t t = 0; t < sectionCount; t++) {

		const uint32_t itemCount = vehicleVaues->getSectionItemCount(t);
		for (uint32_t i = 0; i < itemCount; i++) {
			int32_t itemID = vehicleVaues->getItemID(t, i);
			if (!vehicleVaues->checkIfDescrOnly(t, i)) {
				if (!vehicleVaues->updateValue(dataFields.at(t).at(itemID - 1)->GetValue().ToStdString(), t, i)) {
					wxString errMesg("Data field has bad format, please correct it in \"");
					errMesg.append(vehicleVaues->getSectionTitle(t));
					errMesg.append("\" at line ");
					errMesg.append(std::to_string(itemID));
					wxMessageBox(errMesg, "Error Message", wxICON_WARNING | wxOK);
					return false;
				}
			}
		}
	}
	return true;
}

void cMain::OnAbout(wxCommandEvent& evt) {
	// avoid starting multiple instances of the window
	if (about && !about->IsBeingDeleted())
		about->Destroy();
	
	about = new wAbout(this);
	about->Show();
}

void cMain::OnValueChange(wxCommandEvent& evt) {
	dataChanged = true;
	evt.Skip();
}
