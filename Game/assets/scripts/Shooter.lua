function OnCreate()
    clock = 0;
    pos = Self.GetTransform(Self).Transform.Pos
end

function OnUpdate(deltaTime)
    clock = clock + deltaTime

    if clock >= 2 then
        Scene.SpawnProjectile(Scene, pos.x, pos.y, 0, 0)
        clock = 0
    end
end