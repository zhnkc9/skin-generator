
local transfer_set = {
{% for skinid, attrs in  skin_prefabs %}
{% for asset in attrs.assets %}
{% if asset.ext == ".dyn" or asset.ext == ".tex" %}
    "{{asset.path}}|{{asset.originName}}|{{asset.name}}|{{asset.ext}}",
{% endif %}
{% endfor %}
{% endfor %}
{% for skinid, attrs in  clothing %}
{% for asset in attrs.assets %}
{% if asset.ext == ".dyn" or asset.ext == ".tex" %}
    "{{asset.path}}|{{asset.originName}}|{{asset.name}}|{{asset.ext}}",
{% endif %}
{% endfor %}
{% endfor %}
}

function string:split(delimiter)
    local result = {}
    local pattern = string.format("([^%s]+)", delimiter)
    self:gsub(pattern, function(c) table.insert(result, c) end)
    return result
end

local io = GLOBAL.io

function transferInternal()

    for _, v in ipairs(transfer_set) do
        local assets = v:split("|")
        local path = assets[1]
        local origin = assets[2]
        local dest = assets[3]
        local ext = assets[4]

        print("transferInternal:", path, origin, dest, ext)

        local ifs = io.open(path .. origin .. ext, "rw")
        local ofs = io.open(MODROOT .. path .. dest .. ext, "w")
        local byte = ifs:read("*a")
        ofs:write(byte)
        ifs:close()
        ofs:close()
        print("transferInternal: < L ", #byte, " > ", MODROOT .. path .. dest .. ext)
    end
end

function processIfNecessary()
    print("MODROOT:", MODROOT)
    print("io :", io)
    local p = MODROOT .. "install.txt";
    local fs = io.open(MODROOT .. "install.txt", "r")
    local c = fs and fs:read() or "";
    print("[ Local Skin Collection ] processIfNecessary:", p, c)
    if fs then fs:close() end;
    c = c and c:gsub("%s", "") or c
    if not c or c == "" or c == "0" then
        print("[ Local Skin Collection ] first running  ... ")
        transferInternal()
        fs = io.open(p, "w")
        fs:write("3");
        fs:close()
    end
end

processIfNecessary()

GLOBAL.setfenv(1, GLOBAL)

function abigail_flower_init_fn (inst, build_name)
    if not TheWorld.ismastersim then
        return
    end
    if inst.flower_skin_id then
        inst.flower_skin_id:set(inst.skin_id)
    end
    inst.AnimState:SetSkin(build_name, "abigail_flower_rework")
    inst.components.inventoryitem:ChangeImageName(inst:GetSkinName())
    inst.linked_skinname = string.gsub(build_name, "_flower", "")
end
