#pragma once

#include <shared/UnicodeSupport.h>

std::tstring BrowseForFolder(HWND hWndOwner, std::tstring const& title);
std::tstring BrowseForOutputFile(HWND hWndOwner, std::tstring const& title, std::tstring const& filter, std::tstring const& defaultExtension);
