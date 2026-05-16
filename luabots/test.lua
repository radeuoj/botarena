name = "Tudor"

-- map is 900x600
-- robot has a circle hitbox with radius 50
-- a bullet has radius 5 and speed 30 per tick and does 10 damage

function init()
	print("INIT")
end

function update() -- 20 ticks per second
	go(10); -- clamps between -10 and 10
	turn(2 * math.pi) -- clamps between -PI / 20 and PI / 20
	turnGun(2 * math.pi) -- clamps between -PI / 10 and PI / 10
	turnRadar(-2 * math.pi) -- clamps between -PI / 6 and PI / 6
	shoot() -- only once every 20 ticks
	print(positionX) -- shows you position and rotation after the last frame
	print(positionY) -- does not update after you call turn
	print(rotation)
	print(gunRotation) -- relative to rotation
	print(radarRotation) -- relative to rotation
	print(health) -- 0..100
	print(tick) -- tick count
end

-- the radar is a line and reports the closest hit to you
-- hitX and hitY are the coords of the hit robot
-- this is called after update
function onRadarHit(hitX, hitY)
	print(string.format("HIT: %f %f", hitX, hitY))
end