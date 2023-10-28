_G = GLOBAL
require = _G.require

print("Is MiM enabled?: ", is_mim_enabled, " What is the server game mode?: ", _G.TheNet:GetServerGameMode())
if is_mim_enabled and _G.TheNet:GetServerGameMode() ~= "" then return end

modimport("scripts/init")

Assets = {}

local DEFAULT_PREFIX = "{{prefix}}"

function string:start_with_that_prefix()
    return self:sub(1, #DEFAULT_PREFIX) == DEFAULT_PREFIX
end

function string:trip_that_prefix()
    return self:sub(#DEFAULT_PREFIX + 1)
end

modimport("skinloader/skinloader")
modimport("localskinmain")

initializeModMain()
