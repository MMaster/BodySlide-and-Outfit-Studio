/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius & MMaster
See the included LICENSE file
*/

#include "NOAGroupAssigner.h"

wxBEGIN_EVENT_TABLE(NOAGroupAssigner, wxDialog)
	EVT_FILEPICKER_CHANGED(XRCID("m_filePickerGroups"), NOAGroupAssigner::OnGroupChange)
	EVT_COMBOBOX_CLOSEUP(XRCID("m_comboPresetNude"), NOAGroupAssigner::OnPresetNudeChange)
	EVT_COMBOBOX_CLOSEUP(XRCID("m_comboPresetOutfit"), NOAGroupAssigner::OnPresetOutfitChange)
	EVT_COMBOBOX_CLOSEUP(XRCID("m_comboPresetArmor"), NOAGroupAssigner::OnPresetArmorChange)
	EVT_BUTTON(XRCID("m_buttonReload"), NOAGroupAssigner::OnReload)
	EVT_BUTTON(wxID_SAVE, NOAGroupAssigner::OnSave)
	EVT_BUTTON(wxID_CANCEL, NOAGroupAssigner::OnCancel)
	EVT_CLOSE(NOAGroupAssigner::OnClose)
wxEND_EVENT_TABLE()

NOAGroupAssigner::NOAGroupAssigner(wxWindow* parent, std::vector<std::string> outfits, BodySlideApp* app) {
	wxXmlResource *xrc = wxXmlResource::Get();
	xrc->Load("res\\xrc\\NOAGroupAssigner.xrc");
	xrc->LoadDialog(this, parent, "dlgNoaGroupAssigner");

	wxLogMessage("NOA init");
	this->app = app;
	SetSize(800, 600);
	SetDoubleBuffered(true);
	CenterOnParent();

	presetNude = XRCCTRL(*this, "m_comboPresetNude", wxComboBox);
	presetOutfit = XRCCTRL(*this, "m_comboPresetOutfit", wxComboBox);
	presetArmor = XRCCTRL(*this, "m_comboPresetArmor", wxComboBox);

	scrollWindow = XRCCTRL(*this, "m_scrollWindow", wxScrolledWindow);
	fpGroups = XRCCTRL(*this, "m_filePickerGroups", wxFilePickerCtrl);
	fpGroups->SetInitialDirectory("SliderGroups");
	btnReload = XRCCTRL(*this, "m_buttonReload", wxButton);
	wxFileName fileInfo("SliderGroups\\NOA.xml");
	fileName = fileInfo.GetFullPath();
	groupNameNude = "NOA (nude)";
	groupNameOutfit = "NOA (outfit)";
	groupNameArmor = "NOA (armor)";
	allOutfits = outfits;
	InitUI();
}

NOAGroupAssigner::~NOAGroupAssigner() {
	wxXmlResource::Get()->Unload("res\\xrc\\NOAGroupAssigner.xrc");
}

void NOAGroupAssigner::InitUI() {
	checkNude.clear();
	checkOutfit.clear();
	checkArmor.clear();

	std::vector<std::string> presets;
	app->GetPresetNames(presets);

	for (std::string preset : presets) {
		presetNude->AppendString(preset);
		presetOutfit->AppendString(preset);
		presetArmor->AppendString(preset);
	}

	std::vector<wxString> choicesOutput;
	std::vector<wxArrayString> choicesList;
	std::map<std::string, std::vector<std::string>> outFileSets;

	app->GetOutputFileSets(outFileSets);

	for (auto &filePath : outFileSets) {
		if (filePath.second.size() > 1) {
			wxArrayString selFilePaths;
			for (auto &file : filePath.second) {
				// only outfits from our filtered outfits
				if (find(allOutfits.begin(), allOutfits.end(), file) != allOutfits.end())
					selFilePaths.Add(wxString::FromUTF8(file));
			}

			if (selFilePaths.Count() > 0) {
				choicesList.push_back(selFilePaths);
				choicesOutput.push_back(filePath.first);
			}
		}
	}

	wxSizer *sizerOutfits = scrollWindow->GetSizer();
	sizerOutfits->Clear();

	int nChoice = 1;

	for (auto &choices : choicesList) {
		wxRadioBox* choiceBox = new wxRadioBox(scrollWindow, wxID_ANY, wxString::Format("%3d. ", nChoice) + choicesOutput[nChoice - 1], wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
		sizerOutfits->Add(choiceBox, 0, wxALL | wxEXPAND, 5);
		choiceBoxesOutfits.push_back(choiceBox);

		wxCheckBox* checkBox;
		checkBox = new wxCheckBox(scrollWindow, wxID_ANY, "Nude");
		checkBox->SetValue(true);
		checkBox->Enable(false);
		checkBox->Bind(wxEVT_CHECKBOX, &NOAGroupAssigner::OnNudeCheckChanged, this);
		sizerOutfits->Add(checkBox, 0, wxALL | wxALIGN_CENTER, 5);
		checkNude.push_back(checkBox);

		checkBox = new wxCheckBox(scrollWindow, wxID_ANY, "Outfit");
		checkBox->Bind(wxEVT_CHECKBOX, &NOAGroupAssigner::OnOutfitCheckChanged, this);
		sizerOutfits->Add(checkBox, 0, wxALL | wxALIGN_CENTER, 5);
		checkOutfit.push_back(checkBox);

		checkBox = new wxCheckBox(scrollWindow, wxID_ANY, "Armor");
		checkBox->Bind(wxEVT_CHECKBOX, &NOAGroupAssigner::OnArmorCheckChanged, this);
		sizerOutfits->Add(checkBox, 0, wxALL | wxALIGN_CENTER, 5);
		checkArmor.push_back(checkBox);

		wxButton* previewButton;
		previewButton = new wxButton(scrollWindow, wxID_ANY, "Preview");
		previewButton->Bind(wxEVT_BUTTON, &NOAGroupAssigner::OnPreviewClick, this);
		sizerOutfits->Add(previewButton, 0, wxALL | wxALIGN_CENTER, 5);
		previewButtons.push_back(previewButton);

		nChoice++;
	}

	scrollWindow->FitInside();

	RefreshUI();
}

void NOAGroupAssigner::RefreshUI() {
}

void NOAGroupAssigner::ReloadUI() {
	wxLogMessage("NOA ReloadUI");
	int idx = 0;
	for (auto &cb : choiceBoxesOutfits) {
		checkNude[idx]->SetValue(false);
		checkOutfit[idx]->SetValue(false);
		checkArmor[idx]->SetValue(false);

		wxArrayString choiceOutfits = cb->GetStrings();
		for (wxString outfit : choiceOutfits) {

			std::map<std::string, std::vector<std::string>>::iterator it;
			std::vector<std::string> members;

			it = groupMembers.find(groupNameNude);
			if (it != groupMembers.end()) {
				members = it->second;
				
				if (find(members.begin(), members.end(), outfit.ToStdString()) != members.end()) {
					cb->SetStringSelection(outfit);
					SetCheckboxes(idx, true, false, false);
					break;
				}
			}

			it = groupMembers.find(groupNameOutfit);
			if (it != groupMembers.end()) {
				members = it->second;

				if (find(members.begin(), members.end(), outfit.ToStdString()) != members.end()) {
					cb->SetStringSelection(outfit);
					SetCheckboxes(idx, false, true, false);
					break;
				}
			}

			it = groupMembers.find(groupNameArmor);
			if (it != groupMembers.end()) {
				members = it->second;

				if (find(members.begin(), members.end(), outfit.ToStdString()) != members.end()) {
					cb->SetStringSelection(outfit);
					SetCheckboxes(idx, false, false, true);
					break;
				}
			}
		}
		if (!checkNude[idx]->IsChecked() && !checkOutfit[idx]->IsChecked() && !checkArmor[idx]->IsChecked()) {
			wxString output = cb->GetLabel();

			size_t last_slash = output.find_last_of('\\');
			wxString part = output.substr(last_slash + 1).MakeLower();
			if (part.Contains("_arm") ||
				part.Contains("_leg") ||
				part.StartsWith("toros_") ||
				part.Contains("arm_f") ||
				part.Contains("leg_f") ||
				part.Contains("torso") ||
				part.Contains("arml") ||
				part.Contains("armr") ||
				part.Contains("legl") ||
				part.Contains("legr") ||
				part.Contains("helmet") ||
				part.StartsWith("heavy") ||
				part.StartsWith("chest") ||
				part.StartsWith("farmor")
				)
				SetCheckboxes(idx, false, false, true);
			else
				SetCheckboxes(idx, true, false, false);

			cb->SetBackgroundColour(wxColour(255, 200, 200));
		}
		else {
			cb->SetBackgroundColour(checkNude[idx]->GetBackgroundColour());
		}
		idx++;
	}
}

void NOAGroupAssigner::RemoveOutfitFromGroup(std::string groupName, std::string outfit)
{
	std::vector<std::string>::iterator it;

	it = find(groupMembers[groupName].begin(), groupMembers[groupName].end(), outfit);
	if (it != groupMembers[groupName].end()) {
		groupMembers[groupName].erase(it);
	}
}

void NOAGroupAssigner::RemoveFromAllGroups(int idx) {
	auto &cb = choiceBoxesOutfits[idx];
	auto outfits = cb->GetStrings();

	for (auto &outfit : outfits) {
		std::string outfit_str = outfit.ToStdString();
		RemoveOutfitFromGroup(groupNameNude, outfit_str);
		RemoveOutfitFromGroup(groupNameOutfit, outfit_str);
		RemoveOutfitFromGroup(groupNameArmor, outfit_str);
	}
}

void NOAGroupAssigner::SaveGroup() {
	// update group members (leaving existing groups and members not part of selection intact)
	for (int idx = 0; idx < choiceBoxesOutfits.size(); idx++) {
		// first remove all outfits of this choice
		RemoveFromAllGroups(idx);

		// add selected outfit to correct group
		AddGroupsMember(idx);
	}

	SliderSetGroupFile groupFile;
	wxFileName fileInfo(fileName);
	fileName = fileInfo.GetFullPath();
	groupFile.New(fileName.ToUTF8().data());
	for (auto &grp : groupMembers) {
		SliderSetGroup group;
		group.SetName(grp.first);
		group.AddMembers(grp.second);
		groupFile.UpdateGroup(group);
	}
	groupFile.Save();
	dirty = false;
}

void NOAGroupAssigner::OnGroupChange(wxFileDirPickerEvent& event) {
	wxFileName fileInfo(event.GetPath());
	fileName = fileInfo.GetFullPath();

	std::string groupName;

	size_t lastdot = fileName.find_last_of(".");
	if (lastdot == std::string::npos)
		groupName = fileName;
	else
		groupName = fileName.substr(0, lastdot);

	size_t lastslash = groupName.find_last_of("\\");
	if (lastslash != std::string::npos)
		groupName = groupName.substr(lastslash + 1);

	XRCCTRL(*this, "m_staticText10", wxStaticText)->SetLabel("Group: " + groupName);

	groupNameNude = groupName + " (nude)";
	groupNameOutfit = groupName + " (outfit)";
	groupNameArmor = groupName + " (armor)";
}

void NOAGroupAssigner::OnPresetNudeChange(wxCommandEvent &)
{
}

void NOAGroupAssigner::OnPresetOutfitChange(wxCommandEvent &)
{
}

void NOAGroupAssigner::OnPresetArmorChange(wxCommandEvent &)
{
}

void NOAGroupAssigner::SetCheckboxes(int idx, bool nude, bool outfit, bool armor) {
	checkNude[idx]->SetValue(nude);
	checkNude[idx]->Enable(nude != true);
	checkOutfit[idx]->SetValue(outfit);
	checkOutfit[idx]->Enable(outfit != true);
	checkArmor[idx]->SetValue(armor);
	checkArmor[idx]->Enable(armor != true);
}

void NOAGroupAssigner::AddGroupsMember(int idx)
{
	std::string outfit = choiceBoxesOutfits[idx]->GetStringSelection().ToStdString();

	if (checkNude[idx]->IsChecked()) {
		groupMembers[groupNameNude].push_back(outfit);
		return;
	} 

	if (checkOutfit[idx]->IsChecked()) {
		groupMembers[groupNameOutfit].push_back(outfit);
		return;
	}

	if (checkArmor[idx]->IsChecked()) {
		groupMembers[groupNameArmor].push_back(outfit);
		return;
	}
}

void NOAGroupAssigner::OnPreviewClick(wxCommandEvent &event) {
	wxButton* btn = (wxButton*)event.GetEventObject();
	if (!btn)
		return;
	
	ptrdiff_t idx = distance(previewButtons.begin(), find(previewButtons.begin(), previewButtons.end(), btn));
	if (checkNude[idx]->IsChecked())
		app->ActivatePreset(presetNude->GetValue().ToStdString(), false);
	else
		if (checkOutfit[idx]->IsChecked())
			app->ActivatePreset(presetOutfit->GetValue().ToStdString(), false);
		else
			if (checkArmor[idx]->IsChecked())
				app->ActivatePreset(presetArmor->GetValue().ToStdString(), false);
	app->ActivateOutfit(choiceBoxesOutfits[idx]->GetStringSelection().ToStdString());
	app->ShowPreview();
}

void NOAGroupAssigner::OnNudeCheckChanged(wxCommandEvent &event)
{
	wxCheckBox* cb = (wxCheckBox*)event.GetEventObject();
	if (!cb)
		return;

	ptrdiff_t idx = distance(checkNude.begin(), find(checkNude.begin(), checkNude.end(), cb));
	if (cb->IsChecked()) {
		SetCheckboxes(idx, true, false, false);
	}
	dirty = true;
}

void NOAGroupAssigner::OnOutfitCheckChanged(wxCommandEvent &event)
{
	wxCheckBox* cb = (wxCheckBox*)event.GetEventObject();
	if (!cb)
		return;

	ptrdiff_t idx = distance(checkOutfit.begin(), find(checkOutfit.begin(), checkOutfit.end(), cb));
	if (cb->IsChecked()) {
		SetCheckboxes(idx, false, true, false);
	}
	dirty = true;
}

void NOAGroupAssigner::OnArmorCheckChanged(wxCommandEvent &event)
{
	wxCheckBox* cb = (wxCheckBox*)event.GetEventObject();
	if (!cb)
		return;

	ptrdiff_t idx = distance(checkArmor.begin(), find(checkArmor.begin(), checkArmor.end(), cb));
	if (cb->IsChecked()) {
		SetCheckboxes(idx, false, false, true);
	}
	dirty = true;
}

void NOAGroupAssigner::OnReload(wxCommandEvent& WXUNUSED(event)) {
	wxLogMessage("NOA Reload Pressed");
	// Load group file
	SliderSetGroupFile groupFile(fileName.ToUTF8().data());
	if (groupFile.fail())
		return;

	groupMembers.clear();


	// Fill group member map
	std::vector<std::string> groupNames;
	groupFile.GetGroupNames(groupNames, true, true);
	for (auto &grp : groupNames) {
		wxLogMessage(wxString::Format("Processing group: %s", grp));
		SliderSetGroup group;
		if (groupFile.GetGroup(grp, group)) {
			wxLogError(wxString::Format("Group '%s' not found in file?!", grp));
			return;
		}

		std::vector<std::string> members;
		group.GetMembers(members);
		groupMembers[grp] = members;
	}

	dirty = false;
	ReloadUI();
}

void NOAGroupAssigner::OnSave(wxCommandEvent &)
{
	SaveGroup();
}

void NOAGroupAssigner::OnCancel(wxCommandEvent &)
{
	Close();
}

void NOAGroupAssigner::OnClose(wxCloseEvent& event) {
	if (dirty) {
		int ret = wxMessageBox(_("Save changes?"), _("Save Changes"), wxYES_NO | wxCANCEL | wxICON_WARNING, this);
		if (ret == wxCANCEL) {
			event.Veto();
			return;
		}
		else if (ret == wxYES)
			SaveGroup();
	}

	EndModal(wxID_OK);
}
