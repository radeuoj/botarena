-- Tudor Competitive Robot
-- Strategy: continuous radar sweep, predictive aiming, perpendicular strafing with jitter,
-- and simple hit-avoidance movement. Works within the provided API.

name = "Copilot"

-- persistent tracking
lastSeenX = nil
lastSeenY = nil
lastSeenTick = nil
targetVX = 0
targetVY = 0

-- tuning
bulletSpeed = 30 -- given by game
maxDrive = 8     -- chosen drive speed (clamped by engine)
circleBias = 1.0 -- how strongly we strafe perpendicular to target
jitterStrength = 0.6 -- small oscillation to avoid perfect patterns

function init()
    -- initialize any state
    lastSeenX = nil
    lastSeenY = nil
    lastSeenTick = nil
    targetVX = 0
    targetVY = 0
end

-- normalize angle to range -pi..pi
local function normalize(a)
    while a > math.pi do a = a - 2 * math.pi end
    while a < -math.pi do a = a + 2 * math.pi end
    return a
end

-- distance between two points
local function dist(x1, y1, x2, y2)
    local dx = x2 - x1
    local dy = y2 - y1
    return math.sqrt(dx * dx + dy * dy)
end

function update()
    -- Radar: continuous wide sweep to find nearest opponent
    -- turnRadar accepts values clamped by engine; we sweep full speed
    turnRadar(math.pi / 6) -- sweep right fast; engine clamps

    -- default movement: gentle forward motion with jitter to avoid being an easy target
    local driveSpeed = maxDrive
    local jitter = math.sin(tick * 0.15) * jitterStrength
    go(driveSpeed + jitter)

    -- if we have a last seen target, try to aim and strafe
    if lastSeenX then
        -- compute absolute angle to last seen position
        local angleToTarget = math.atan(lastSeenY - positionY, lastSeenX - positionX)
        local distanceToTarget = dist(positionX, positionY, lastSeenX, lastSeenY)

        -- Predictive aiming: estimate lead point using last velocity estimate
        local leadTime = 0
        if bulletSpeed > 0 then
            leadTime = distanceToTarget / bulletSpeed
        end
        local leadX = lastSeenX + targetVX * leadTime
        local leadY = lastSeenY + targetVY * leadTime
        local aimAngle = math.atan(leadY - positionY, leadX - positionX)

        -- Turn body slowly toward target but prefer strafing perpendicular
        -- We set body turn to slowly face the target (helps movement direction)
        local bodyTurn = normalize(angleToTarget - rotation) * 0.6
        turn(bodyTurn)

        -- Strafing: move perpendicular to the target to make our path harder to hit
        -- We implement strafing by turning the body slightly off-target and keeping forward speed
        local strafeAngle = angleToTarget + math.pi / 2 * circleBias * ( (tick % 40 < 20) and 1 or -1 )
        local desiredHeading = normalize(strafeAngle - rotation)
        turn(desiredHeading * 0.25)

        -- Gun aiming: aim at the predictive aimAngle
        local absoluteGunAngle = rotation + gunRotation
        local gunTurn = normalize(aimAngle - absoluteGunAngle)
        turnGun(gunTurn)

        -- Fire when gun is roughly aligned and radar recently saw the target
        -- We check a small tolerance so we don't waste shots
        local gunAligned = math.abs(gunTurn) < 0.08
        if gunAligned then
            shoot()
        end
    end

    -- keep gun and radar moving a bit to pick up new targets
    turnGun(0.02)
    turnRadar(-0.05)
end

-- called after update when radar hits the closest robot
function onRadarHit(hitX, hitY)
    -- compute dt and update velocity estimate
    if lastSeenX and lastSeenTick and lastSeenTick ~= tick then
        local dt = tick - lastSeenTick
        if dt > 0 then
            targetVX = (hitX - lastSeenX) / dt
            targetVY = (hitY - lastSeenY) / dt
        end
    else
        -- first sighting, assume stationary until we get a second reading
        targetVX = 0
        targetVY = 0
    end

    -- store latest sighting
    lastSeenX = hitX
    lastSeenY = hitY
    lastSeenTick = tick

    -- immediate micro-evasion if opponent is very close
    local d = math.sqrt((hitX - positionX)^2 + (hitY - positionY)^2)
    if d < 160 then
        -- back off and strafe unpredictably
        go(-8)
        turn(math.pi / 10 * ((tick % 2 == 0) and 1 or -1))
        turnGun(normalize(math.atan(hitY - positionY, hitX - positionX) - (rotation + gunRotation)) * 0.8)
        -- try to shoot if gun is aligned
        shoot()
    end
end
