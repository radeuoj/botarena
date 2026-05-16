name = "Gemini"

-- State tracking
local enemyX = nil
local enemyY = nil
local lastSeenTick = -100
local radarSweepDir = 1
local moveDir = 1
local lastShotTick = -20

-- Helper function to keep angles cleanly within -PI to PI
function normalizeAngle(angle)
    while angle > math.pi do angle = angle - 2 * math.pi end
    while angle < -math.pi do angle = angle + 2 * math.pi end
    return angle
end

-- Helper function to get distance between two points
function getDistance(x1, y1, x2, y2)
    return math.sqrt((x2 - x1)^2 + (y2 - y1)^2)
end

function init()
    print("Apex online. Hunting started.")
    math.randomseed(42) -- Seed the random number generator
end

function update()
    local currentTick = tick
    
    -- ==========================================
    -- 1. RADAR CONTROL (The "Wobble Lock")
    -- ==========================================
    if currentTick - lastSeenTick > 3 then
        -- If we lost the enemy, sweep the radar at maximum speed
        turnRadar((math.pi / 6) * radarSweepDir)
    else
        -- We know where they are. Point the radar directly at them.
        local angleToEnemy = math.atan(enemyY - positionY, enemyX - positionX)
        local absoluteRadarAngle = rotation + radarRotation
        local radarTurnNeeded = normalizeAngle(angleToEnemy - absoluteRadarAngle)
        
        -- Add a slight "wobble" (over-turning). Because the enemy is moving, 
        -- perfectly pointing at them might cause us to lose them next tick.
        -- Over-turning ensures the radar beam sweeps back and forth across their hitbox.
        if radarTurnNeeded >= 0 then
            turnRadar(radarTurnNeeded + 0.1)
        else
            turnRadar(radarTurnNeeded - 0.1)
        end
    end

    -- ==========================================
    -- 2. MOVEMENT & POSITIONING
    -- ==========================================
    if enemyX ~= nil then
        local angleToEnemy = math.atan(enemyY - positionY, enemyX - positionX)
        local distanceToEnemy = getDistance(positionX, positionY, enemyX, enemyY)

        -- Goal 1: Stay perpendicular to the enemy (orbit them) to dodge bullets
        local desiredHeading = angleToEnemy + (math.pi / 2)
        
        -- Goal 2: Maintain a safe distance (closer than 400, further than 150)
        -- If too far, angle slightly toward them. If too close, angle slightly away.
        if distanceToEnemy > 400 then
            desiredHeading = desiredHeading - (0.5 * moveDir)
        elseif distanceToEnemy < 150 then
            desiredHeading = desiredHeading + (0.5 * moveDir)
        end

        -- Goal 3: Wall Avoidance (Map is 900x600)
        local margin = 80
        if positionX < margin or positionX > 900 - margin or positionY < margin or positionY > 600 - margin then
            local angleToCenter = math.atan(300 - positionY, 450 - positionX)
            -- Override heading to smoothly steer back to the middle
            desiredHeading = angleToCenter
        end

        -- Randomly reverse direction to throw off predictive aiming bots
        if math.random() < 0.02 then -- ~2% chance every tick
            moveDir = moveDir * -1
        end

        -- Turn the body toward our desired heading
        local bodyTurnNeeded = normalizeAngle(desiredHeading - rotation)
        
        -- Optimization: If the turn is more than 90 degrees, it's faster to go backward
        if math.abs(bodyTurnNeeded) > math.pi / 2 then
            bodyTurnNeeded = normalizeAngle(bodyTurnNeeded + math.pi)
            go(-10 * moveDir)
        else
            go(10 * moveDir)
        end
        
        turn(bodyTurnNeeded)
        
    else
        -- If we've never seen an enemy, go to the center and spin
        local angleToCenter = math.atan(300 - positionY, 450 - positionX)
        turn(normalizeAngle(angleToCenter - rotation))
        go(10)
    end

    -- ==========================================
    -- 3. WEAPONS & AIMING
    -- ==========================================
    if enemyX ~= nil then
        local angleToEnemy = math.atan(enemyY - positionY, enemyX - positionX)
        local absoluteGunAngle = rotation + gunRotation
        local gunTurnNeeded = normalizeAngle(angleToEnemy - absoluteGunAngle)

        turnGun(gunTurnNeeded)

        -- Fire control: Only shoot if the gun is mostly aimed at the enemy
        -- and the weapon isn't on cooldown (1 shot per 20 ticks)
        if math.abs(gunTurnNeeded) < 0.1 and currentTick - lastSeenTick < 5 then
            if currentTick - lastShotTick >= 20 then
                shoot()
                lastShotTick = currentTick
            end
        end
    end
end

function onRadarHit(hitX, hitY)
    enemyX = hitX
    enemyY = hitY
    lastSeenTick = tick
    
    -- Reversing the sweep direction ensures that if the target escapes 
    -- the radar beam, the radar immediately swings back to catch them.
    radarSweepDir = -radarSweepDir 
end