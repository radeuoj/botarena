name = "Sniper_v2"

local targetX = nil
local targetY = nil
local lastHitTick = 0
local radarDirection = 1

function init()
    turnRadar(0.5)
end

function update()
    if tick - lastHitTick > 3 then
        targetX = nil
        targetY = nil
    end

    if targetX ~= nil and targetY ~= nil then
        local dx = targetX - positionX
        local dy = targetY - positionY
        local angleToTarget = math.atan2(dy, dx)

        local relativeAngle = angleToTarget - rotation
        while relativeAngle > math.pi do relativeAngle = relativeAngle - (2 * math.pi) end
        while relativeAngle < -math.pi do relativeAngle = relativeAngle + (2 * math.pi) end

        local relativeGunAngle = angleToTarget - (rotation + gunRotation)
        while relativeGunAngle > math.pi do relativeGunAngle = relativeGunAngle - (2 * math.pi) end
        while relativeGunAngle < -math.pi do relativeGunAngle = relativeGunAngle + (2 * math.pi) end

        turnGun(relativeGunAngle)

        if math.abs(relativeGunAngle) < 0.1 then
            shoot()
        end

        if tick % 10 < 5 then
            go(4)
            turn(0.05)
        else
            go(-2)
            turn(-0.05)
        end

        local relativeRadarAngle = angleToTarget - (rotation + radarRotation)
        while relativeRadarAngle > math.pi do relativeRadarAngle = relativeRadarAngle - (2 * math.pi) end
        while relativeRadarAngle < -math.pi do relativeRadarAngle = relativeRadarAngle + (2 * math.pi) end
        
        turnRadar(relativeRadarAngle + (0.05 * radarDirection))
        if tick % 4 == 0 then radarDirection = -radarDirection end

    else
        turnRadar(0.3)
        
        if positionX < 100 or positionX > 800 or positionY < 100 or positionY > 500 then
            turn(0.1)
            go(3)
        else
            go(5)
        end
    end
end

function onRadarHit(hitX, hitY)
    targetX = hitX
    targetY = hitY
    lastHitTick = tick
end