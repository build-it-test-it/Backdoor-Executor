local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Enabled = false
ScreenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling
ScreenGui.Parent = game:GetService("StarterGui")

local Loading = Instance.new("Frame")
Loading.Name = "Loading"
Loading.Size = UDim2.new(0.00, 396.00, 0.00, 261.00)
Loading.BorderColor3 = Color3.new(0.00, 0.00, 0.00)
Loading.Position = UDim2.new(0.32, 0.00, 0.27, 0.00)
Loading.BorderSizePixel = 0
Loading.BackgroundColor3 = Color3.new(0.62, 0.00, 0.00)
Loading.Parent = ScreenGui

local R = Instance.new("Frame")
R.Name = "R"
R.Size = UDim2.new(1.00, 0.00, 1.00, 0.00)
R.BorderColor3 = Color3.new(0.00, 0.00, 0.00)
R.BorderSizePixel = 0
R.BackgroundColor3 = Color3.new(0.00, 0.00, 0.00)
R.Parent = Loading

local TextLabel = Instance.new("TextLabel")
TextLabel.TextWrapped = true
TextLabel.BorderSizePixel = 0
TextLabel.RichText = true
TextLabel.BackgroundColor3 = Color3.new(1.00, 1.00, 1.00)
TextLabel.FontFace = Font.new("rbxasset://fonts/families/SourceSansPro.json", Enum.FontWeight.Regular, Enum.FontStyle.Normal)
TextLabel.TextSize = 100
TextLabel.Size = UDim2.new(1.00, 0.00, 1.00, 0.00)
TextLabel.BorderColor3 = Color3.new(0.00, 0.00, 0.00)
TextLabel.Text = "Loading.."
TextLabel.TextColor3 = Color3.new(1.00, 1.00, 1.00)
TextLabel.BackgroundTransparency = 1
TextLabel.Parent = Loading

local LocalScript = Instance.new("LocalScript")
LocalScript.Source = "game.CoreGui:FindFirstChild(\'ScriptHub\', true):FindFirstAncestorOfClass(\'Folder\'):Destroy()\nwhile wait() do\n	local ts = game:GetService(\'TweenService\')\n	ts:Create(script.Parent, TweenInfo.new(2), {TextTransparency=0.8}):Play()\n	wait(2)\n	ts:Create(script.Parent, TweenInfo.new(2), {TextTransparency=0}):Play()\n	wait(2)\nend"
LocalScript.Parent = TextLabel
