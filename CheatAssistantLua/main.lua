local path = 'C:/lua/';
--package.path = path..'Other\\?.lua;'..path..'Data\\?.lua;'..path..'Script\\?.lua;'
dofile(path.."Other/define.lua")
memory = dofile(path.."Other/memory.lua")
dofile(path.."Data/global.lua")
dofile(path.."Data/baseAddress.lua")
status_0 = dofile(path.."Script/status_0.lua")
status_1 = dofile(path.."Script/status_1.lua")
status_2 = dofile(path.."Script/status_2.lua")
status_3 = dofile(path.."Script/status_3.lua")
sendPacket = dofile(path.."Script/sendPacket.lua")
fun = dofile(path.."Script/fun.lua")
call = dofile(path.."Script/call.lua")
hook = dofile(path.."Script/hook.lua")
knapsac = dofile(path.."Script/knapsac.lua")
task = dofile(path.."Script/task.lua")
role = dofile(path.."Script/role.lua")
utils = dofile(path.."Script/utils.lua")
dofile(path.."Script/astar.lua")

g_hWnd = findWindow("地下城与勇士","地下城与勇士")
-- print(g_hWnd)
g_gameWindowInfo = getWindowRect(g_hWnd)
-- utils:printTable(g_gameWindowInfo)

function onKeyDown(wParam)
	if(wParam == HotKeyId_F1)
		then
		local rolePos = role:getRolePos();
		print("当前位置",rolePos.x,rolePos.y)
		x = rolePos.x+500
		y = rolePos.y+200
		role:moveRoleToPos(x,y)
		print("目标位置",x,y)
		rolePos = role:getRolePos();
		print("实际到达位置",rolePos.x,rolePos.y)
	elseif(wParam == HotKeyId_F2)
		then
		status_3:getTheSpoils()
	elseif(wParam == HotKeyId_F3)
		then
		status_3:outputMapObjectInfo()
	elseif(wParam == HotKeyId_F4)
		then
		role:moveRoleToPos(613, 255);
	elseif(wParam == HotKeyId_F5)
		then
		print(role:getRolePos().x,role:getRolePos().y)
	elseif(wParam == HotKeyId_F6)
		then
		-- status_1:赛利亚房间移动到格蓝迪门口2()
		print(status_3:getObjectPointerByCode(Code_通关营火))
	elseif(wParam == HotKeyId_F7)
		then
		print(status_3:getTheSpoilsCount())
	elseif(wParam == HotKeyId_F9)
		then
		-- print(knapsac:getGoodsCount())
		status_3:outputMapObjectInfo()
		print(status_3:getTheSpoilsCount())
		
	elseif(wParam == HotKeyId_END)
		then
		g_自动开关 = not g_自动开关;
	end
end


function threadMessage(uMsg,wParam,lParam)
	outputDebugString(string.format("Lua uMsg->:%d,wParam->:%x,lParam->:%x",uMsg,wParam,lParam))
	onKeyDown(wParam)
end

function main()
	local gameStatus
	-- print("自动已开启")
	while(luaSwitch(g_自动开关))
		do
		
		while(luaSwitch(g_自动开关))
			do
			gameStatus = fun:getGameStatus()
			if(gameStatus == 3)
				then
				status_3:manage()
			elseif(gameStatus == 2)
				then
				status_2:manage()
			elseif(gameStatus == 1)
				then
				status_1:manage()
			elseif(gameStatus == 0)
				then
				status_0:manage()
			end
		end
		print("自动已关闭")
		break;
	end
	sleep(500)
end

