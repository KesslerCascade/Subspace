local subspace = subspace
local lua = lua
local execfile = lua.execfile

if subspace.dev then
    execfile("debug.lua")
end

function startup()
    print("Starting up!")
    local label = iup.label { title = "Test 123" }
    local closebutton = iup.button { title = "&Close", cpadding = "6x3" }

    closebutton.action = function()
        subspace.exit()
    end

    local consolebutton = iup.button { title = "C&onsole", cpadding = "6x3" }
    consolebutton.action = function()
        execfile("console.lua")
        show_console()
    end

    local vb = iup.vbox { label, iup.hbox { closebutton, consolebutton, gap = 10 } }
    vb.margin = "10x10"
    local dlg = iup.dialog { vb }
    dlg:map()
    dlg:show(iup.CENTER, iup.CENTER)
end

function shutdown()
    print("Shutting down!")
end