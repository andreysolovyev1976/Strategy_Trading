//
// Created by Andrey Solovyev on 17/04/2022.
//

#pragma once

#include "user_defined_strings.h"
#include "ui_events.h"
#include <vector>

#ifndef STRATEGY_TRADING_UI_MAIN_MENU_H
#define STRATEGY_TRADING_UI_MAIN_MENU_H

namespace user_interface {
  using namespace std::literals;

  using MenuLabel = types::String;
  using MenuItems = std::vector<types::String>;
  using MainMenu = std::map<MenuLabel, MenuItems>;
  using MainMenuEvents = std::map<MenuLabel, Event>;

  static const MenuItems standard_menu_items {"Add", "List", "Remove"};
  static const MainMenu main_menu{
		  {"Indicators", standard_menu_items},
		  {"Signals", standard_menu_items},
		  {"Rules", standard_menu_items},
		  {"Strategies", {"Add", "List", "Remove", "Start", "Stop"}},
		  {"Contracts", standard_menu_items},

  };
  static const MainMenuEvents main_menu_events {
		  {"Indicators", Event::setup_Indicators},
		  {"Signals", Event::setup_Signals},
		  {"Rules", Event::setup_Rules},
		  {"Strategies", Event::setup_Strategies},
		  {"Contracts", Event::setup_Contracts},
  };

}//!namespace

#endif //STRATEGY_TRADING_UI_MAIN_MENU_H
