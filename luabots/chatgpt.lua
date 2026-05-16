name = "ChatGPT"

local MAP_W, MAP_H = 900, 600
local BULLET_SPEED = 30
local MAX_SENSOR_AGE = 999999

local target = {
	x = nil,
	y = nil,
	vx = 0,
	vy = 0,
	lastSeen = -MAX_SENSOR_AGE
}

local sweepDir = 1
local orbitDir = 1

local function clamp(v, lo, hi)
	if v < lo then return lo end
	if v > hi then return hi end
	return v
end

local function normAngle(a)
	while a > math.pi do a = a - 2 * math.pi end
	while a < -math.pi do a = a + 2 * math.pi end
	return a
end

local function atan2(y, x)
	if x > 0 then
		return math.atan(y / x)
	elseif x < 0 and y >= 0 then
		return math.atan(y / x) + math.pi
	elseif x < 0 and y < 0 then
		return math.atan(y / x) - math.pi
	elseif x == 0 and y > 0 then
		return math.pi / 2
	elseif x == 0 and y < 0 then
		return -math.pi / 2
	end
	return 0
end

local function dist(x1, y1, x2, y2)
	local dx = x2 - x1
	local dy = y2 - y1
	return math.sqrt(dx * dx + dy * dy)
end

local function wallRepulsion()
	local margin = 120
	local rx, ry = 0, 0

	if positionX < margin then
		rx = rx + (margin - positionX) / margin
	elseif positionX > MAP_W - margin then
		rx = rx - (positionX - (MAP_W - margin)) / margin
	end

	if positionY < margin then
		ry = ry + (margin - positionY) / margin
	elseif positionY > MAP_H - margin then
		ry = ry - (positionY - (MAP_H - margin)) / margin
	end

	return rx, ry
end

function init()
	print("INIT")
end

function update()
	local seenRecently = (tick - target.lastSeen) <= 12
	local haveTarget = target.x ~= nil and target.y ~= nil and seenRecently

	local moveAngle

	if haveTarget then
		local d = dist(positionX, positionY, target.x, target.y)

		-- Lead the target based on its estimated velocity.
		local leadTime = d / BULLET_SPEED
		local aimX = target.x + target.vx * leadTime
		local aimY = target.y + target.vy * leadTime

		local aimAngle = atan2(aimY - positionY, aimX - positionX)
		local gunErr = normAngle(aimAngle - (rotation + gunRotation))
		local radarErr = normAngle(aimAngle - (rotation + radarRotation))

		-- Keep radar locked on the target.
		turnRadar(clamp(radarErr * 1.6, -math.pi / 6, math.pi / 6))

		-- Orbit the target while adapting distance.
		if d < 220 then
			orbitDir = -1
		elseif d > 380 then
			orbitDir = 1
		end

		local strafeAngle = aimAngle + orbitDir * math.pi / 2

		-- Add wall avoidance so we do not get pinned in a corner.
		local rx, ry = wallRepulsion()
		local mx = math.cos(strafeAngle) + rx * 2.2
		local my = math.sin(strafeAngle) + ry * 2.2
		moveAngle = atan2(my, mx)

		turn(normAngle(moveAngle - rotation))
		go(10)

		-- Fire when the gun is lined up well enough.
		if math.abs(gunErr) < 0.06 and d < 800 then
			shoot()
		end
	else
		-- Patrol toward the center while sweeping radar.
		local centerAngle = atan2((MAP_H / 2) - positionY, (MAP_W / 2) - positionX)
		local rx, ry = wallRepulsion()

		local patrolX = math.cos(centerAngle + 0.35 * math.sin(tick / 30)) + rx * 2.5
		local patrolY = math.sin(centerAngle + 0.35 * math.sin(tick / 30)) + ry * 2.5
		moveAngle = atan2(patrolY, patrolX)

		turn(normAngle(moveAngle - rotation))
		go(8)

		-- Continuous sweep until we find someone.
		turnRadar((math.pi / 6) * sweepDir)

		-- Flip sweep direction occasionally to avoid dead zones.
		if tick % 120 == 0 then
			sweepDir = -sweepDir
		end
	end
end

function onRadarHit(hitX, hitY)
	local dt = tick - target.lastSeen
	if dt <= 0 then dt = 1 end

	if target.x ~= nil and target.y ~= nil and dt > 0 and dt < 100 then
		target.vx = (hitX - target.x) / dt
		target.vy = (hitY - target.y) / dt
	end

	target.x = hitX
	target.y = hitY
	target.lastSeen = tick
end