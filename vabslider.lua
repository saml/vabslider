local mouseX = 0;
local mouseY = 0;

local function onTick(event)
	local x;
	local y;
	
	x, y = mp.get_mouse_pos();
	if x ~= mouseX or y ~= mouseY then
		print(x, y);
		mouseX, mouseY = x, y;
		-- mp.command('vf-command xcrop x ' .. mouseX .. '"');
		-- mp.commandv('vf-command', 'crop', 'w', '' .. mouseX .. '');
		-- mp.commandv('vf-command', 'crop', 'out_w', '100');
		mp.command('vf-command crop out_w 100');
	end
end

mp.register_event('tick', onTick);
-- mp.set_property('lavfi-complex', '[vid1][vid2]scale2ref[main][ref]; [main]crop@xcrop=w=200:x=0:y=0:keep_aspect=1[cropped]; [ref][cropped]overlay[vo]');
