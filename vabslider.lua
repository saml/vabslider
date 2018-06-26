local _M = {
	vid1=nil,
	vid2=nil,
	larger=nil,
	smaller=nil,
	lavfi=nil,
};



local function resetSlider()
	local x, y = mp.get_mouse_pos();
	local width,height,aspect = mp.get_osd_size();
	print('osd size', width, height, aspect);
	local left,top,right,bottom = mp.get_osd_margins();
	print('osd margins', left, top, right, bottom);
	local scaled_width = width - right - left;
	local video_width = _M.smaller['demux-w'];
	print(string.format('osd=%sx%s (%s) ltrb=(%s,%s,%s,%s) width=%s scaled=%s', width,height,aspect,left,top,right,bottom, video_width, scaled_width));
	local scaled_x = x - left;
	local video_x = video_width * scaled_x / scaled_width;
	local crop_width;
	if video_x <= 0 then
		crop_width = 5;
	elseif video_x >= video_width then
		crop_width = video_width;
	else
		crop_width = video_x;
	end
	-- print(string.format('mouse=(%s,%s) osd-size=%s osd-margins=%s video_width=%s scaled_width=%s video_x=%s crop=%s', x, y, osd_width, left, video_width, scaled_width, video_x, crop_width));
	mp.set_property('lavfi-complex', string.format(_M.lavfi, crop_width));
		
end


local function main(event)
	local vid1 = mp.get_property_native('track-list/0');
	local vid2 = mp.get_property_native('track-list/1');
	_M.vid1 = vid1;
	_M.vid2 = vid2;
	if vid1['demux-w'] * vid1['demux-h'] < vid2['demux-w'] * vid2['demux-h'] then
		_M.smaller = vid1;
		_M.larger = vid2;
		_M.lavfi = '[vid2][vid1]scale2ref[second][first]; [first]crop=w=%s:x=0:y=0[cropped]; [second][cropped]overlay[vo]';
	else
		_M.smaller = vid2;
		_M.larger = vid1;
		_M.lavfi = '[vid1][vid2]scale2ref[second][first]; [first]crop=w=%s:x=0:y=0[cropped]; [second][cropped]overlay[vo]';
	end
	mp.set_property('lavfi-complex', string.format(_M.lavfi, _M.smaller['demux-w'] / 2));
end
	
mp.register_event('file-loaded', main);
mp.add_key_binding('z', 'resetSlider', resetSlider);
-- mp.register_event('mbtn_left', onClick);
-- mp.set_property('lavfi-complex', '[vid1][vid2]scale2ref[main][ref]; [main]crop@xcrop=w=200:x=0:y=0:keep_aspect=1[cropped]; [ref][cropped]overlay[vo]');
