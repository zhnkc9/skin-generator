-- len {{ size(clothing) }}
local groupid = 0825
local _ =
{
{% for skinid, attrs in  clothing %}
    {{ prefix + skinid }} =
    {
{% for key, value in attrs %}
{% if key == "assets" %}
        {{ key }} = {
{% for asset in value %}
            {{ asset2str(asset) }},
{% endfor %}
        },
{% else if key == "build_name_override"  %}
        {{ key  }} = {{ dump(attrs.build_name_override) }} ,
{% else if key == "skins" %}
        skins = { {% for sk,sv in value %}{{ sk }} = "{{sv}}" , {% endfor %} },
{% else if key == "origin_build_name" %}
{% else %}
        {{ key }} = {{ value  }},
{% endif %}
{% endfor %}
    },
{% endfor %}
}
return _
