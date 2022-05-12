function normalize(val, max, min)
    return (val - min) / (max - min)
end

function OnCreate()
    player = Scene.FindByName(Scene, "Player")
    YSpeed = 300
    XSpeed = 300
    
    target = {}
    target.x = player.GetTransform(player).Transform.Pos.x - Self.GetTransform(Self).Transform.Pos.x
    target.y = player.GetTransform(player).Transform.Pos.y - Self.GetTransform(Self).Transform.Pos.y
    
    len = math.sqrt(target.x^2 + target.y^2)
    
    direction = {}
    direction.x = target.x / len
    direction.y = target.y / len
    
    clock = 0
end 

function OnUpdate(deltaTime)
    clock = clock + deltaTime
    if clock >= 3 then
        Scene.DestroyActor(Scene, Self)
        print("Destroyed!")
    end
    Self.GetTransform(Self).Transform.Pos.x = Self.GetTransform(Self).Transform.Pos.x + XSpeed * direction.x * deltaTime;
    Self.GetTransform(Self).Transform.Pos.y = Self.GetTransform(Self).Transform.Pos.y + YSpeed * direction.y * deltaTime;
    
end 