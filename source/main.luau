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

local UIGradient = Instance.new("UIGradient")
UIGradient.Transparency = NumberSequence.new(NumberSequenceKeypoint.new(0.00, 0.73, 0.00), NumberSequenceKeypoint.new(0.06, 0.74, 0.00), NumberSequenceKeypoint.new(0.06, 1.00, 0.00), NumberSequenceKeypoint.new(0.13, 1.00, 0.00), NumberSequenceKeypoint.new(0.13, 0.74, 0.00), NumberSequenceKeypoint.new(0.21, 0.74, 0.00), NumberSequenceKeypoint.new(0.21, 0.99, 0.00), NumberSequenceKeypoint.new(0.32, 1.00, 0.00), NumberSequenceKeypoint.new(0.32, 0.77, 0.00), NumberSequenceKeypoint.new(0.41, 0.75, 0.00), NumberSequenceKeypoint.new(0.41, 1.00, 0.00), NumberSequenceKeypoint.new(0.54, 1.00, 0.00), NumberSequenceKeypoint.new(0.54, 0.77, 0.00), NumberSequenceKeypoint.new(0.70, 0.77, 0.00), NumberSequenceKeypoint.new(0.70, 1.00, 0.00), NumberSequenceKeypoint.new(0.94, 0.98, 0.00), NumberSequenceKeypoint.new(0.94, 0.76, 0.00), NumberSequenceKeypoint.new(1.00, 0.76, 0.00), NumberSequenceKeypoint.new(1.00, 1.00, 0.00), NumberSequenceKeypoint.new(1.00, 0.76, 0.00))
UIGradient.Rotation = 45
UIGradient.Parent = R

local L = Instance.new("Frame")
L.Name = "L"
L.Size = UDim2.new(1.00, 0.00, 1.00, 0.00)
L.BorderColor3 = Color3.new(0.00, 0.00, 0.00)
L.BorderSizePixel = 0
L.BackgroundColor3 = Color3.new(0.00, 0.00, 0.00)
L.Parent = Loading

local UIGradient_1 = Instance.new("UIGradient")
UIGradient_1.Transparency = NumberSequence.new(NumberSequenceKeypoint.new(0.00, 0.73, 0.00), NumberSequenceKeypoint.new(0.06, 0.74, 0.00), NumberSequenceKeypoint.new(0.06, 1.00, 0.00), NumberSequenceKeypoint.new(0.13, 1.00, 0.00), NumberSequenceKeypoint.new(0.13, 0.74, 0.00), NumberSequenceKeypoint.new(0.21, 0.74, 0.00), NumberSequenceKeypoint.new(0.21, 0.99, 0.00), NumberSequenceKeypoint.new(0.32, 1.00, 0.00), NumberSequenceKeypoint.new(0.32, 0.77, 0.00), NumberSequenceKeypoint.new(0.41, 0.75, 0.00), NumberSequenceKeypoint.new(0.41, 1.00, 0.00), NumberSequenceKeypoint.new(0.54, 1.00, 0.00), NumberSequenceKeypoint.new(0.54, 0.77, 0.00), NumberSequenceKeypoint.new(0.70, 0.77, 0.00), NumberSequenceKeypoint.new(0.70, 1.00, 0.00), NumberSequenceKeypoint.new(0.94, 0.98, 0.00), NumberSequenceKeypoint.new(0.94, 0.76, 0.00), NumberSequenceKeypoint.new(1.00, 0.76, 0.00), NumberSequenceKeypoint.new(1.00, 1.00, 0.00), NumberSequenceKeypoint.new(1.00, 0.76, 0.00))
UIGradient_1.Rotation = -45
UIGradient_1.Parent = L

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


