name = "Canta"

ticks_left = 40
dir = 1

function update()
	if ticks_left == 0 then 
		dir = -dir
		ticks_left = 40
	end

	go(10 * dir)
	turnGun(math.pi / 10)
	turnRadar(math.pi / 10)
	ticks_left = ticks_left - 1
end

function onRadarHit(hitX, hitY)
	shoot()
end