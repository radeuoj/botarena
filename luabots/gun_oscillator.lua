name = "gun oscillator"

function update()
	go(10);
	turn(2 * math.pi)
	turnGun(math.cos(os.clock() * 10))
	shoot()
end