-- len {{ size(skin_prefabs) }}
local prefs = {}
local groupid = 0825

{% for skinid, attrs in  skin_prefabs %}
table.insert(prefs, CreatePrefabSkin("{{ prefix + skinid }}",{
{% for key, value in attrs %}
{% if key == "assets" %}
    {{ key }} = {
{% for asset in value %}
        {{ asset2str(asset) }},
{% endfor %}
    },
{% else if key == "build_name_override"  %}
    {{ key  }} = {{  dump(value) }} ,
{% else if key == "base_prefab" and (value == "\"campfire\"" or value == "\"lantern\"") %}
    {{ key  }} = {{ value }} ,
    init_fn = {{ attrs.init_fn }},
{% else if key == "skins" %}
    skins = { {% for sk,sv in value %}{{ sk }} = "{{sv}}" , {% endfor %} },
{% else if key == "origin_skins" or key == "origin_build_name" or key == "bigportrait_anim" or key == "init_fn"%}
{% else %}
    {{ key }} = {{ value  }},
{% endif %}
{% endfor %}
}))

{% endfor %}
return unpack(prefs)
