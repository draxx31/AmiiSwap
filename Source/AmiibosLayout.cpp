#include "MainApplication.hpp"

extern lang::Language *language;

namespace ui
{
    extern MainApplication *mainapp;

    AmiibosLayout::AmiibosLayout()
    {
		this->amiibosMenu = pu::ui::elm::Menu::New(0, 80, 1280, pu::ui::Color(255,255,255,255), 60, 10);
        this->amiibosMenu->SetOnFocusColor(pu::ui::Color(102,153,204,255));
        this->amiibosMenu->SetScrollbarColor(pu::ui::Color(102,153,204,255));
        this->amiibosMenu->SetVisible(true);
        this->Add(this->amiibosMenu);
        //this->SetElementOnFocus(this->amiibosMenu);
        this->SetOnInput(std::bind(&AmiibosLayout::manage_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

	void AmiibosLayout::manage_Input(u64 Down, u64 Up, u64 Held)
	{
		if (Down & KEY_B){
            this->amiibosMenu->ClearItems();
            mainapp->SetFooterText(lang::GetLabel(lang::Label::FOOTER_GAMES) + std::to_string(set::GetGamesSize()));
            mainapp->SetHelpText(lang::GetLabel(lang::Label::HELP_SELECT) + lang::GetLabel(lang::Label::HELP_ADD) + lang::GetLabel(lang::Label::HELP_MANAGE) + lang::GetLabel(lang::Label::HELP_MINUS));
            //mainapp->GetGamesLayout()->SetElementOnFocus(mainapp->GetGamesLayout()->GetGamesMenu());
            mainapp->GetGamesLayout()->GetGamesMenu()->SetVisible(true);
            mainapp->LoadLayout(mainapp->GetGamesLayout());
		}
	}

    void AmiibosLayout::populateAmiibosMenu(amiibo::Game *game)
    {
        for (auto amiibo : game->GetAmiibos()) {
            this->amiibosMenu->AddItem(CreateItem(amiibo));
        }
		this->amiibosMenu->SetSelectedIndex(0);
    }

    pu::ui::elm::MenuItem::Ref &AmiibosLayout::CreateItem(std::string amiibo)
    {
        std::string amiiboName = amiibo;
        std::string amiiboPath = "sdmc:/emuiibo/" + amiibo;
        size_t size = amiibo.find_last_of("/\\");
        if (size != std::string::npos)
            amiiboName = amiibo.substr(size + 1);
        pu::ui::elm::MenuItem::Ref item = pu::ui::elm::MenuItem::New(amiiboName);

        item->SetIcon(amiiboPath + "/amiibo.icon");
        item->AddOnClick(std::bind(&AmiibosLayout::selectItem_Click, this, amiibo), KEY_A);
        item->AddOnClick(std::bind(&AmiibosLayout::randomizeUuid_Click, this, amiibo), KEY_X);
        return item;
    }

    void AmiibosLayout::selectItem_Click(std::string amiibo)
    {
        std::string amiiboName = amiibo;
        std::string amiiboPath = "sdmc:/emuiibo/" + amiibo;
        size_t size = amiibo.find_last_of("/\\");
        if (size != std::string::npos)
            amiiboName = amiibo.substr(size + 1);
        int sopt = mainapp->CreateShowDialog(utils::replace(lang::GetLabel(lang::Label::SELECT_DIALOG_TITLE), "{{AMIIBO_NAME}}", amiiboName), utils::replace(lang::GetLabel(lang::Label::SELECT_DIALOG_TEXT), "{{AMIIBO_NAME}}", amiiboName), { lang::GetLabel(lang::Label::YES), lang::GetLabel(lang::Label::NO) }, true, amiiboPath + "/amiibo.icon");
        if (sopt == 0) {
            nfpemuSetCustomAmiibo(amiiboPath.c_str());
            pu::ui::extras::Toast::Ref toast = pu::ui::extras::Toast::New(utils::replace(lang::GetLabel(lang::Label::TOAST_ACTIVE_AMIIBO), "{{AMIIBO_NAME}}", amiiboName), 20, pu::ui::Color(255,255,255,255), pu::ui::Color(0,51,102,255));
            mainapp->StartOverlayWithTimeout(toast, 1500);
        }
    }

    void AmiibosLayout::randomizeUuid_Click(std::string amiibo)
    {
    	std::string amiiboName = amiibo;
        std::string amiiboPath = "sdmc:/emuiibo/" + amiibo;
        std::string jsonPath = amiiboPath + "/amiibo.json";
        size_t size = amiibo.find_last_of("/\\");
        if (size != std::string::npos)
            amiiboName = amiibo.substr(size + 1);
        bool randomStatus = utils::isRandomUuid(jsonPath);
		int sopt = mainapp->CreateShowDialog(lang::GetLabel(lang::Label::RANDOM_DIALOG_TITLE), utils::replace(utils::replace(lang::GetLabel(lang::Label::RANDOM_DIALOG_TEXT), "{{AMIIBO_NAME}}", amiiboName), "{{STATUS}}", (randomStatus ? lang::GetLabel(lang::Label::ENABLED) : lang::GetLabel(lang::Label::DISABLED))), { lang::GetLabel(lang::Label::TOGGLE), lang::GetLabel(lang::Label::CANCEL) }, true, amiiboPath + "/amiibo.icon");
        if(sopt == 0){
            toggleRandomUuid(jsonPath, !randomStatus);
            pu::ui::extras::Toast::Ref toast = pu::ui::extras::Toast::New(utils::replace(utils::replace(lang::GetLabel(lang::Label::TOAST_RANDOM), "{{AMIIBO_NAME}}", amiiboName), "{{STATUS}}", (randomStatus ? lang::GetLabel(lang::Label::ENABLED) : lang::GetLabel(lang::Label::DISABLED))), 20, pu::ui::Color(255,255,255,255), pu::ui::Color(0,51,102,255));
            mainapp->StartOverlayWithTimeout(toast, 1500);
        }
    }

    void AmiibosLayout::toggleRandomUuid(std::string jsonPath, bool toggle)
    {
        std::ifstream ifs(jsonPath);
        auto amiiboJson = json::parse(ifs);
        if(ifs.is_open())
            ifs.close();
        amiiboJson["randomizeUuid"] = toggle;
        std::ofstream ofs(jsonPath);
        ofs << amiiboJson.dump(4);
        if(ofs.is_open())
            ofs.close();
    }

    pu::ui::elm::Menu::Ref AmiibosLayout::GetAmiibosMenu()
    {
    	return (this->amiibosMenu);
    }
}
