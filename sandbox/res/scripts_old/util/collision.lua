point_intersects = function(transform, aabb, point)
	
	local x = transform.position.x + aabb.offset.x
	local y = transform.position.y + aabb.offset.y

	local minx = x - aabb.half_extent.x
	local miny = y - aabb.half_extent.y
	local maxx = x + aabb.half_extent.x
	local maxy = y + aabb.half_extent.y

	if point.x > minx and point.x < maxx and point.y > miny and point.y < maxy then
		return true
	end
	return false
end