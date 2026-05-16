name = "Claude"

-- ── State ────────────────────────────────────────────────────────────────────
local enemyX, enemyY           = nil, nil   -- last known enemy position
local prevEnemyX, prevEnemyY   = nil, nil   -- position one reading ago
local lastSpotTick             = -999        -- tick of last radar hit
local prevSpotTick             = -999        -- tick of reading before that

local strafeDir   =  1   -- +1 or -1: which side we're strafing
local strafeTimer =  0   -- counts up; flip direction at threshold
local radarDir    =  1   -- sweep direction when searching

-- ── Helpers ──────────────────────────────────────────────────────────────────
local function clamp(v, lo, hi)
    return math.max(lo, math.min(hi, v))
end

-- Wrap angle into (-π, π]
local function normAngle(a)
    while a >  math.pi do a = a - 2 * math.pi end
    while a < -math.pi do a = a + 2 * math.pi end
    return a
end

-- ── Init ─────────────────────────────────────────────────────────────────────
function init()
    print("Nemesis online")
end

-- ── Update ───────────────────────────────────────────────────────────────────
function update()
    local WALL_MARGIN = 110
    local nearWall = positionX < WALL_MARGIN or positionX > 900 - WALL_MARGIN
                  or positionY < WALL_MARGIN or positionY > 600 - WALL_MARGIN

    local hasTarget = enemyX ~= nil and (tick - lastSpotTick) < 60

    -- ── Strafing timer (change direction every ~2.5 s) ──────────────────────
    strafeTimer = strafeTimer + 1
    if strafeTimer > 50 then
        strafeDir   = -strafeDir
        strafeTimer = 0
    end

    -- ════════════════════════════════════════════════════════════════════════
    --  COMBAT MODE
    -- ════════════════════════════════════════════════════════════════════════
    if hasTarget then

        local dx   = enemyX - positionX
        local dy   = enemyY - positionY
        local dist = math.sqrt(dx * dx + dy * dy)
        local angleToEnemy = math.atan(dy, dx)

        -- ── 1. Movement: strafe perpendicular to enemy ───────────────────────
        if nearWall then
            -- Escape the wall: turn toward map center
            local toCenter = math.atan(300 - positionY, 450 - positionX)
            local diff = normAngle(toCenter - rotation)
            turn(clamp(diff, -math.pi / 20, math.pi / 20))
        else
            local strafeAngle = angleToEnemy + (math.pi / 2) * strafeDir
            local diff = normAngle(strafeAngle - rotation)
            turn(clamp(diff, -math.pi / 20, math.pi / 20))
        end
        go(10)

        -- ── 2. Predictive gun aim ────────────────────────────────────────────
        -- Estimate enemy velocity from the last two radar readings
        local travelTicks = dist / 30   -- ticks until bullet arrives
        local predX, predY = enemyX, enemyY

        if prevEnemyX ~= nil then
            local age = lastSpotTick - prevSpotTick
            if age > 0 then
                local velX = (enemyX - prevEnemyX) / age
                local velY = (enemyY - prevEnemyY) / age
                predX = enemyX + velX * travelTicks
                predY = enemyY + velY * travelTicks
            end
        end

        local aimAngle    = math.atan(predY - positionY, predX - positionX)
        local absGunAngle = rotation + gunRotation
        local gunDiff     = normAngle(aimAngle - absGunAngle)
        turnGun(clamp(gunDiff, -math.pi / 10, math.pi / 10))

        -- Fire when the barrel is roughly on target
        if math.abs(gunDiff) < 0.12 then
            shoot()
        end

        -- ── 3. Radar: lock on, tiny oscillation to avoid losing contact ──────
        local absRadarAngle = rotation + radarRotation
        local radarDiff     = normAngle(angleToEnemy - absRadarAngle)
        -- Bias the sweep slightly ahead of where the enemy was moving
        local leadBias = (prevEnemyX ~= nil) and
            math.atan((enemyY - prevEnemyY), (enemyX - prevEnemyX)) * 0.05 or 0
        turnRadar(clamp(radarDiff + leadBias, -math.pi / 6, math.pi / 6))

    -- ════════════════════════════════════════════════════════════════════════
    --  SEARCH MODE
    -- ════════════════════════════════════════════════════════════════════════
    else
        -- Spin radar at full speed, circle slowly, stay off walls
        turnRadar(math.pi / 6 * radarDir)

        if nearWall then
            local toCenter = math.atan(300 - positionY, 450 - positionX)
            local diff = normAngle(toCenter - rotation)
            turn(clamp(diff, -math.pi / 20, math.pi / 20))
        else
            turn(math.pi / 20)
        end
        go(8)
    end
end

-- ── Radar callback ───────────────────────────────────────────────────────────
function onRadarHit(hitX, hitY)
    -- Roll the position history forward
    prevEnemyX  = enemyX
    prevEnemyY  = enemyY
    prevSpotTick = lastSpotTick

    enemyX      = hitX
    enemyY      = hitY
    lastSpotTick = tick

    -- Flip radar oscillation so it bounces back and forth over the target
    radarDir = -radarDir
end