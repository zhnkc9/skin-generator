local env = env
local AddClassPostConstruct = AddClassPostConstruct

_G.setfenv(1, _G)

require("misc_items")

local AccountItemFrame = require("widgets/redux/accountitemframe")
local ItemImage = require("widgets/itemimage")
local Image = require("widgets/image")
local official_on_tex = "button_officialskins_off.tex"
local official_off_tex = "button_officialskins_on.tex"
local modded_on_tex = "button_moddedskins_off.tex"
local modded_off_tex = "button_moddedskins_on.tex"

local _SetTexture = Image.SetTexture

local SetRarity = AccountItemFrame._SetRarity
local SetEventIcon = AccountItemFrame._SetEventIcon
local _SetActivityState = AccountItemFrame.SetActivityState

local _SetItem = ItemImage.SetItem
Image.SetTexture = function(self, atlas, tex, default_tex)
    if default_tex == nil and atlas == "images/button_icons.xml"
            and (tex == official_on_tex
            or tex == official_off_tex
            or tex == modded_on_tex
            or tex == modded_off_tex) then
        atlas = "images/ms_buttons.xml"
    end
    _SetTexture(self, atlas, tex, default_tex)
end
AddClassPostConstruct("widgets/redux/filterbar", function(self)
    local officialfilterID = "officialFilter"
    local moddedfilterID = "moddedFilter"
    local official_filter = function(skin_id)
        if IsDefaultSkin(skin_id) then return true end
        return env.IsAddedSkin(skin_id)
    end
    self.official_filter = self.picker.header:AddChild(self:AddFilter(STRINGS.UI.WARDROBESCREEN.OFFICIAL_FILTER_FMT,
            official_on_tex, official_off_tex, officialfilterID, official_filter))
    local modded_filter = function(skin_id)
        if IsDefaultSkin(skin_id) then return true end
        return not env.IsAddedSkin(skin_id)
    end
    self.modded_filter = self.picker.header:AddChild(self:AddFilter(STRINGS.UI.WARDROBESCREEN.MODDED_FILTER_FMT,
            modded_on_tex, modded_off_tex, moddedfilterID, modded_filter))
    local __UpdatePositions = self._UpdatePositions
    self._UpdatePositions = function(self)
        __UpdatePositions(self)
        if self.hasmigrated then return end

        if self.search_box then
            self.hasmigrated = true
            if not self.thin_mode then
                self.picker.header:Nudge(Vector3(-145, 0, 0))
                self.picker.progress:Nudge(Vector3(145, 0, 0))
                self.official_filter:Nudge(Vector3(70, -65, 0))
                self.modded_filter:Nudge(Vector3(0, -120, 0))
            else
                self.picker.header:Nudge(Vector3(-121, 0, 0))
                self.picker.progress:Nudge(Vector3(130, 0, 0))
                self.search_box:Nudge(Vector3(-20, 0, 0))
                self.official_filter:Nudge(Vector3(46, -65, 0))
                self.modded_filter:Nudge(Vector3(-12, -120, 0))
            end
        elseif self.picker.primary_item_type == "loading" then
            self.hasmigrated = true

            self.picker.header:Nudge(Vector3(-145, 0, 0))
            self.picker.progress:Nudge(Vector3(145, 0, 0))

            self.inst:DoTaskInTime(0,
                    function()
                        self.official_filter:Nudge(Vector3(140, -630, 0))
                        self.modded_filter:Nudge(Vector3(140, -630, 0))
                    end)
        end
    end
end)

function AccountItemFrame:_SetRarity(rarity)
    if rarity == "ModMade" or rarity == "ModLocked" then
        self:GetAnimState():OverrideSymbol("SWAP_frameBG", "modded_frame_BG", GetFrameSymbolForRarity(rarity))
    else
        SetRarity(self, rarity)
    end
end

function AccountItemFrame:_SetEventIcon(item_key)
    local rarity = GetRarityForItem(item_key)
    if rarity == "ModMade" or rarity == "ModLocked" then
        self.is_displaying_mod = true
        self:GetAnimState():OverrideSymbol("DLC", "modded_event_icon", "event_modded")
        self:GetAnimState():Show("DLC")
    else
        self.is_displaying_mod = false
        self:GetAnimState():ClearOverrideSymbol("DLC")
        self:GetAnimState():Hide("DLC")
    end
    SetEventIcon(self, item_key)
end

function AccountItemFrame:SetActivityState(is_active, is_owned, is_unlockable, is_dlc_owned)
    is_unlockable = self.is_displaying_mod and true or is_unlockable
    _SetActivityState(self, is_active, is_owned, is_unlockable, is_dlc_owned)
    if self.is_displaying_mod then self:GetAnimState():Show("DLC") end
end

function ItemImage:SetItem(...)
    _SetItem(self, ...)
    if self.rarity == "ModMade" or self.rarity == "ModLocked" then
        self.frame:GetAnimState():OverrideSymbol("SWAP_frameBG", "modded_frame_BG", GetFrameSymbolForRarity(self.rarity))
    end
end

STRINGS.UI.WARDROBESCREEN.OFFICIAL_FILTER_FMT = "Hide Official Skins: {mode}"
STRINGS.UI.WARDROBESCREEN.MODDED_FILTER_FMT = "Hide Modded Skins: {mode}"
STRINGS.UI.WARDROBESCREEN.MODLOCKEDREQUIREMENTS = "Skin Unlock Requirements"
STRINGS.UI.WARDROBESCREEN.MODLOCKEDREQUIREMENTS_CLOSE = "Close"
STRINGS.UI.WARDROBESCREEN.MODLINK_MOREINFO = "Source Mod"
