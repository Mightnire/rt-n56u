#!/usr/bin/env python3
import sys

if len(sys.argv) < 2:
    sys.exit(0)

target = sys.argv[1]

# 1. Patch Advanced_SettingBackup_Content.asp
asp_path = target + '/Advanced_SettingBackup_Content.asp'
try:
    with open(asp_path, 'r', encoding='utf-8') as f:
        content = f.read()
    old_call = "location.href='Settings_' + document.form.productid.value + '.CFG';"
    new_call = 'var part = \'<% nvram_get_x("", "active_config_part"); %>\'; var suffix = (part && part != "") ? ("_" + part) : ""; location.href="Settings_" + document.form.productid.value + suffix + ".CFG";'
    if old_call in content:
        content = content.replace(old_call, new_call)
        with open(asp_path, 'w', encoding='utf-8') as f:
            f.write(content)
except Exception as e:
    print("Error patching ASP:", e)

# 2. Patch state.js
state_path = target + '/state.js'
try:
    with open(state_path, 'r', encoding='utf-8') as f:
        content = f.read()
    old_firmver = '\tshowtext($("firmver"), \'<% nvram_get_x("",  "firmver_sub"); %>\');'
    new_firmver = '\tvar fv = \'<% nvram_get_x("",  "firmver_sub"); %>\'; var act_part = \'<% nvram_get_x("", "active_config_part"); %>\'; if (act_part == "Config2") fv += \'&nbsp;<span class="label label-success" title="Slider: Wireless Mode">Config2</span>\'; else if (act_part == "Config") fv += \'&nbsp;<span class="label label-info" title="Slider: Wired Mode">Config</span>\'; $("firmver").innerHTML = fv;'
    if old_firmver in content:
        content = content.replace(old_firmver, new_firmver)
        with open(state_path, 'w', encoding='utf-8') as f:
            f.write(content)
except Exception as e:
    print("Error patching state.js:", e)

print("PR2000 WebUI romfs patch applied successfully!")
