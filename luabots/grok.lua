name = "Grok"

-- State
local targetX = nil
local targetY = nil
local targetTick = 0
local lastScanX = nil
local lastScanY = nil

local desiredSpeed = 8
local mode = "scan"  -- scan, chase, circle

local mapW = 900
local mapH = 600
local radius = 50

function init()
    print("TudorCompetitor initialized")
    math.randomseed(tick or 42)
end

function update()
    -- Basic movement: avoid walls + some randomness
    local distToWall = math.min(
        positionX - radius,
        mapW - positionX - radius,
        positionY - radius,
        mapH - positionY - radius
    )
    
    if distToWall < 120 then
        -- Turn away from nearest wall
        local wallAngle = 0
        if positionX < mapW/2 then wallAngle = 0 else wallAngle = math.pi end
        if positionY < mapH/2 and math.abs(positionY - mapH/2) > math.abs(positionX - mapW/2) then
            wallAngle = math.pi/2
        end
        turn(wallAngle - rotation)
        desiredSpeed = 10
    else
        desiredSpeed = 8 + math.random() * 4 - 2
    end
    
    go(desiredSpeed)
    
    -- Radar sweep
    if mode == "scan" or (tick - targetTick > 40) then
        turnRadar(2 * math.pi)   -- full fast sweep when searching
    else
        -- Lock on target when we have one
        local dx = targetX - positionX
        local dy = targetY - positionY
        local targetAngle = math.atan(dy, dx)
        local relAngle = targetAngle - (rotation + radarRotation)
        relAngle = (relAngle + math.pi) % (2*math.pi) - math.pi
        turnRadar(relAngle * 0.8)  -- strong lock
    end
    
    -- Gun logic
    if targetX then
        local dx = targetX - positionX
        local dy = targetY - positionY
        local dist = math.sqrt(dx*dx + dy*dy)
        
        if dist > 800 then
            targetX = nil  -- too far, lost lock
        else
            -- Simple lead: bullet speed 30, we update 20x/sec
            local timeToHit = dist / 30
            local leadX = targetX + (targetX - (lastScanX or targetX)) * timeToHit * 0.7
            local leadY = targetY + (targetY - (lastScanY or targetY)) * timeToHit * 0.7
            
            dx = leadX - positionX
            dy = leadY - positionY
            local targetGunAngle = math.atan(dy, dx)
            local relGun = targetGunAngle - (rotation + gunRotation)
            relGun = (relGun + math.pi) % (2*math.pi) - math.pi
            
            turnGun(relGun * 1.2)  -- aggressive turn
            
            -- Shoot when aligned
            if math.abs(relGun) < 0.15 and tick % 18 == 0 then  -- slightly under cooldown
                shoot()
            end
        end
    else
        -- Idle gun sweep with radar
        turnGun(2 * math.pi)
    end
    
    -- Body turning - gentle circling when we have a target
    if targetX and tick - targetTick < 80 then
        turn(0.4)  -- circle strafe
    else
        turn(0.8)  -- search turn
    end
end

function onRadarHit(hitX, hitY)
    targetX = hitX
    targetY = hitY
    lastScanX = hitX
    lastScanY = hitY
    targetTick = tick
    mode = "chase"
    
    -- Print for debug
    -- print(string.format("LOCK: %.1f, %.1f  dist=%.1f", hitX, hitY, math.sqrt((hitX-positionX)^2 + (hitY-positionY)^2)))
end