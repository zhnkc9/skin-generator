
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
    if not c or c == "" then
        print("[ Local Skin Collection ] first running  ... ")
        transferInternal()
        fs = io.open(p, "w")
        fs:write("3");
        fs:close()
    end
end

processIfNecessary()
