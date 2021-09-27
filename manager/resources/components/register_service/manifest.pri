PLUGIN_NAME = {{plugin_name}}
PLUGIN_ACTIVATIONPOLICY = eager
PLUGIN_VERSION = {{version}}
PLUGIN_VERSIONDATE = {{version_date}}
PLUGIN_DESCRIPTION = _{{description}}_
PLUGIN_VENDOR = _{{vendor}}_
PLUGIN_CONTACTADDRESS = {{contact}}
PLGUIN_CATEGORY = _{{category}}_
PLUGIN_SERVICECLASS = {{service}}
PLUGIN_DISPLAYNAME = _{{display_name}}_
PLUGIN_TYPE = {{type}}
REQUIRE_PLUGIN ={% for item in dependency %} \
    {{item}}{% endfor %}

