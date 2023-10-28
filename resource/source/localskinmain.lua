local _env = env
_G = GLOBAL
require = _G.require

function mergeA2B(a, b)
    if type(a) ~= "table" or type(b) ~= "table" then return end
    for k, v in pairs(a) do
        b[k] = v
    end
end

PrefabFiles = {
    "kleiskinprefabs",
}

_env._ModdedCurios = {}
mergeA2B(require("clothing_curios"), _env._ModdedCurios)
mergeA2B(require("misc_curios"), _env._ModdedCurios)

require("register_skin")
