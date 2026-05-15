name = "Tudor"

function init()
	print("INIT")
	turn(math.pi / 4)
end

function update()
	go(10);
	turn(2 * math.pi)
end