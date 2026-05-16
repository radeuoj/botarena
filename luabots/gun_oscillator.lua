name = "gun oscillator"

function update()
	go(10);
	turn(2 * math.pi)
	turn_gun(math.cos(os.clock() * 10))
	shoot()
end