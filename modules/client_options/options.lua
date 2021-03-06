Options = {}

local optionsWindow
local optionsButton
local optionsTabBar
local options = { vsync = false,
                  showfps = true,
                  fullscreen = false,
                  classicControl = false,
                  showStatusMessagesInConsole = true,
                  showEventMessagesInConsole = true,
                  showInfoMessagesInConsole = true,
                  showTimestampsInConsole = true,
                  showLevelsInConsole = true,
                  showPrivateMessagesInConsole = false,
                  showPrivateMessagesOnScreen = true,
                  enableMusic = true,
                  showLeftPanel = false }
local generalPanel
local graphicsPanel

local function setupGraphicsEngines()
  local enginesRadioGroup = UIRadioGroup.create()
  local ogl1 = graphicsPanel:getChildById('opengl1')
  local ogl2 = graphicsPanel:getChildById('opengl2')
  enginesRadioGroup:addWidget(ogl1)
  enginesRadioGroup:addWidget(ogl2)

  if g_graphics.getPainterEngine() == 2 then
    enginesRadioGroup:selectWidget(ogl2)
  else
    enginesRadioGroup:selectWidget(ogl1)
  end

  ogl1:setEnabled(g_graphics.isPainterEngineAvailable(1))
  ogl2:setEnabled(g_graphics.isPainterEngineAvailable(2))

  enginesRadioGroup.onSelectionChange = function(self, selected)
    if selected == ogl1 then
      g_graphics.selectPainterEngine(1)
    elseif selected == ogl2 then
      g_graphics.selectPainterEngine(2)
    end
  end

  local foregroundFrameRateScrollBar = graphicsPanel:getChildById('foregroundFrameRateScrollBar')
  local foregroundFrameRateLimitLabel = graphicsPanel:getChildById('foregroundFrameRateLimitLabel')
  if not g_graphics.canCacheBackbuffer() then
    foregroundFrameRateScrollBar:setValue(61)
    foregroundFrameRateScrollBar:disable()
    foregroundFrameRateLimitLabel:disable()
  end
end

function Options.init()
  -- load options
  for k,v in pairs(options) do
    if type(v) == 'boolean' then
      g_settings.setDefault(k, v)
      Options.setOption(k, g_settings.getBoolean(k))
    end
  end

  g_keyboard.bindKeyDown('Ctrl+D', Options.toggle)
  g_keyboard.bindKeyDown('Ctrl+F', function() Options.toggleOption('fullscreen') end)

  optionsWindow = g_ui.displayUI('options.otui')
  optionsWindow:hide()
  optionsButton = TopMenu.addLeftButton('optionsButton', tr('Options') .. ' (Ctrl+D)', 'options.png', Options.toggle)
  optionsTabBar = optionsWindow:getChildById('optionsTabBar')
  optionsTabBar:setContentWidget(optionsWindow:getChildById('optionsTabContent'))

  generalPanel = g_ui.loadUI('general.otui')
  optionsTabBar:addTab(tr('General'), generalPanel)

  graphicsPanel = g_ui.loadUI('graphics.otui')
  optionsTabBar:addTab(tr('Graphics'), graphicsPanel)

  setupGraphicsEngines()
end

function Options.terminate()
  g_keyboard.unbindKeyDown('Ctrl+D')
  g_keyboard.unbindKeyDown('Ctrl+F')
  optionsWindow:destroy()
  optionsWindow = nil
  optionsButton:destroy()
  optionsButton = nil
  optionsTabBar = nil
  generalPanel = nil
  graphicsPanel = nil
  Options = nil
end

function Options.toggle()
  if optionsWindow:isVisible() then
    Options.hide()
  else
    Options.show()
  end
end

function Options.show()
  optionsWindow:show()
  optionsWindow:raise()
  optionsWindow:focus()
end

function Options.hide()
  optionsWindow:hide()
end

function Options.toggleOption(key)
  Options.setOption(key, not Options.getOption(key))
end

function Options.setOption(key, value)
  if key == 'vsync' then
    g_window.setVerticalSync(value)
  elseif key == 'showfps' then
    addEvent(function()
      local frameCounter = rootWidget:recursiveGetChildById('frameCounter')
      if frameCounter then frameCounter:setVisible(value) end
    end)
  elseif key == 'fullscreen' then
    addEvent(function()
      g_window.setFullscreen(value)
    end)
  elseif key == 'enableMusic' then
    addEvent(function()
      g_sounds.enableMusic(value)
    end)
  elseif key == 'showLeftPanel' then
    addEvent(function()
      GameInterface.getLeftPanel():setOn(value)
    end)
  end
  g_settings.set(key, value)
  options[key] = value
end

function Options.getOption(key)
  return options[key]
end
