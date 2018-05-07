/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius & MMaster
See the included LICENSE file
*/

#pragma once

#include "BodySlideApp.h"
#include "../components/SliderGroup.h"
#include "../utils/Log.h"

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/filepicker.h>

class NOAGroupAssigner : public wxDialog {
public:
	NOAGroupAssigner(wxWindow*, std::vector<std::string>, BodySlideApp* app);
	~NOAGroupAssigner();

private:
	BodySlideApp* app;

	wxComboBox* presetNude;
	wxComboBox* presetOutfit;
	wxComboBox* presetArmor;
	wxScrolledWindow* scrollWindow;
	wxFilePickerCtrl* fpGroups;
	wxButton* btnReload;
	std::vector<wxRadioBox*> choiceBoxesOutfits;
	std::vector<wxCheckBox*> checkNude;
	std::vector<wxCheckBox*> checkOutfit;
	std::vector<wxCheckBox*> checkArmor;
	std::vector<wxButton*> previewButtons;

	bool dirty = false;
	wxString fileName;
	std::map<std::string, std::vector<std::string>> groupMembers;
	std::string groupNameNude;
	std::string groupNameOutfit;
	std::string groupNameArmor;
	std::vector<std::string> allOutfits;

	void InitUI();
	void RefreshUI();
	void ReloadUI();
	void RemoveOutfitFromGroup(std::string groupName, std::string outfit);
	void RemoveFromAllGroups(int idx);
	void SaveGroup();

	void OnGroupChange(wxFileDirPickerEvent&);
	void OnPresetNudeChange(wxCommandEvent&);
	void OnPresetOutfitChange(wxCommandEvent&);
	void OnPresetArmorChange(wxCommandEvent&);
	void SetCheckboxes(int idx, bool nude, bool outfit, bool armor);
	void AddGroupsMember(int idx);
	void OnPreviewClick(wxCommandEvent&);
	void OnNudeCheckChanged(wxCommandEvent&);
	void OnOutfitCheckChanged(wxCommandEvent&);
	void OnArmorCheckChanged(wxCommandEvent&);
	void OnReload(wxCommandEvent&);
	void OnSave(wxCommandEvent&);
	void OnCancel(wxCommandEvent&);
	void OnClose(wxCloseEvent&);

	wxDECLARE_EVENT_TABLE();
};
