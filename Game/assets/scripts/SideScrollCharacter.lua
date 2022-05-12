function OnCreate()
    print("Created a new lua script!")
    Attributes = Self.GetAttributes(Self).Attributes
    yMovement = 0
    xMovement = 0
    jumped = false
    firstIteration = true;
end

function math.round(val)
    local lowdiff = val - math.floor(val)
    local highdiff = math.ceil(val) - val
    if highdiff < lowdiff then return math.floor(val)
    else return math.ceil(val) end
end

function math.clamp(val, lower, upper)
    assert(val and lower and upper, "not very useful error message here")
    if lower > upper then lower, upper = upper, lower end -- swap if boundaries supplied the wrong way
    return math.max(lower, math.min(upper, val))
end

function dump(o)
    if type(o) == 'table' then
        local s = '{ '
        for k,v in pairs(o) do
            if type(k) ~= 'number' then k = '"'..k..'"' end
            s = s .. '['..k..'] = ' .. dump(v) .. ','
        end
        return s .. '} '
    else
        return tostring(o)
    end
end

function ValueInRange(value, min, max) 
    return (value >= min) and (value <= max)
end

function KillPlayer()
    Scene.GetGameState(Scene).Lives = Scene.GetGameState(Scene).Lives - 1
    ReturnToSpawn()
end

function Intersects(RectA, RectB)
    
    RectALeft = RectA.Bounds.Left + RectA.Offset.x
    RectATop = RectA.Bounds.Top + RectA.Offset.y
    RectBLeft = RectB.Bounds.Left + RectB.Offset.x
    RectBTop = RectB.Bounds.Top + RectB.Offset.y
    
    xOverlap = ValueInRange(RectALeft, RectBLeft, RectBLeft + RectB.Bounds.Width) or ValueInRange(RectBLeft, RectALeft, RectALeft + RectA.Bounds.Width)
    yOverlap = ValueInRange(RectATop, RectBTop, RectBTop + RectB.Bounds.Height) or ValueInRange(RectBTop, RectATop, RectATop + RectA.Bounds.Height)
    
    return xOverlap and yOverlap
end

function GetSurroundingTiles(tile) 
    local tiles = { 
        [1] = {tile.x + 1, tile.y, tile.Width, tile.Height},
        [2] = {tile.x + 1, tile.y + 1, tile.Width, tile.Height},
        [3] = {tile.x + 1, tile.y - 1, tile.Width, tile.Height},
        [4] = {tile.x - 1, tile.y, tile.Width, tile.Height},
        [5] = {tile.x - 1, tile.y + 1, tile.Width, tile.Height},
        [6] = {tile.x - 1, tile.y - 1, tile.Width, tile.Height},
        [7] = {tile.x, tile.y + 1, tile.Width, tile.Height},
        [8] = {tile.x, tile.y - 1, tile.Width, tile.Height}
    }
    return tiles
end

function GetCollisionBox(tile, tmcoords) 
    local box = {}
    local rect = {}
    local off = {}
    off.x = 0
    off.y = 0
    
    rect.Left = tmcoords.x + (tile[1] * tile[3])
    
    rect.Top = tmcoords.y + (tile[2] * tile[4])
    
    rect.Width = tile[3]
    rect.Height = tile[4]
    
    box.Bounds = rect
    box.Offset = off

    return box
end

function ReturnToSpawn()
    Self.GetTransform(Self).Transform.Pos.x = Attributes.SpawnX
    Self.GetTransform(Self).Transform.Pos.y = Attributes.SpawnY
end

function BeginPlay()
    ReturnToSpawn()
    firstIteration = false;
end

function OnUpdate(deltaTime)
    
    if firstIteration then BeginPlay() end
    yMovement = yMovement + Attributes.Gravity * deltaTime;
    
    if IsMouseDown(0) then
    end
    if IsKeyDown("W") then
        if jumped == false then
            yMovement = Attributes.JumpHeight * -1
            jumped = true
        end
    end
    if IsKeyDown("A") then
        xMovement = xMovement - Attributes.Speed * deltaTime
    end
    if IsKeyDown("S") then
        yMovement = yMovement + (Attributes.JumpHeight * 10) * deltaTime
    end 
    if IsKeyDown("D") then
        xMovement = xMovement + Attributes.Speed * deltaTime
    end
    if not(IsKeyDown("D") and IsKeyDown("A")) then
        xMovement = xMovement * 0.8
    end
    
    local boxcoll = false
    local PlayerBox = Self.GetCollider(Self).CopyBox(Self.GetCollider(Self))
    local FuturePositionX = Self.GetCollider(Self).CopyBox(Self.GetCollider(Self))
    local FuturePositionY = Self.GetCollider(Self).CopyBox(Self.GetCollider(Self))
    FuturePositionY.Bounds.Top = FuturePositionY.Bounds.Top + yMovement
    FuturePositionX.Bounds.Left = FuturePositionX.Bounds.Left + xMovement

    for i,v in ipairs(Scene.GetAllColliders(Scene)) do
        if v.Active then
            if (v ~= Self.GetCollider(Self)) then
                if (Intersects(Self.GetCollider(Self), v) and v.Type == "Overlap") then
                    print ("Overlap!")
                    break
                end
                if (Intersects(Self.GetCollider(Self), v) and v.Type == "Kill") then
                    KillPlayer()
                    yMovement = 0
                    break
                end
                if(Intersects(Self.GetCollider(Self), v) and v.Type == "NextLevel") then
                    
                    local next = Scene.FindByName(Scene, "NextLevel")
                    att = next.GetAttributes(next).Attributes
                    Scene.OpenLevel(Scene, att.NextLevel)
                    
                    break
                end
                if (Intersects(FuturePositionX, v) and v.Type == "Block") then
                    xMovement = 0
                end
                if (Intersects(FuturePositionY, v) and v.Type == "Block") then
                    if yMovement > 0 then
                        jumped = false
                    end
                    yMovement = 0
                    
                end
            end
        end
    end

    
    local PlayerBox = Self.GetCollider(Self)
    
    local TileMaps = Scene.GetAllTilemaps(Scene)
    
    
    for i,TileMap in ipairs(TileMaps) do
        
        mapBounds = {}
        mapOffset = {}
        mapOffset.x = 0
        mapOffset.y = 0
        mapBounds.Bounds = TileMap.Bounds
        mapBounds.Offset = mapOffset
        
        if (Intersects(mapBounds, Self.GetCollider(Self))) then
            
            local PlayerPos = {}
            PlayerPos.x = Self.GetTransform(Self).Transform.Pos.x - TileMap.Bounds.Left
            PlayerPos.y = Self.GetTransform(Self).Transform.Pos.y - TileMap.Bounds.Top
            
            local Tile = {}
            Tile.x = math.floor((PlayerPos.x / TileMap.TileWidth))
            Tile.y = math.floor(PlayerPos.y / TileMap.TileHeight)
            Tile.Width = TileMap.TileWidth
            Tile.Height = TileMap.TileHeight
            
            SurroundingTiles = GetSurroundingTiles(Tile)
            local tilemappos = {}
            tilemappos.x = TileMap.Bounds.Left
            tilemappos.y = TileMap.Bounds.Top
            
            for j,SurroundingTile in ipairs(SurroundingTiles) do
                --print(j)
                local coll = GetCollisionBox(SurroundingTile, tilemappos)

                if Intersects(FuturePositionY, coll) then
                    for k,x in ipairs(TileMap.Colliders) do
                        TestTile = TileMap.Tiles[math.floor(SurroundingTile[1] + (TileMap.Width * SurroundingTile[2]))+1]
                        if TestTile == x then
                            if yMovement > 0 then
                                if true then
                                    boxcoll = true
                                    jumped = false
                                    yMovement = 0
                                    Self.GetTransform(Self).Transform.Pos.y = math.floor(coll.Bounds.Top - (PlayerBox.Bounds.Height/2)) - 0.1
                                end
                            else
                                yMovement = 0
                            end
                            
                        end
                        
                    end
                end
                if Intersects(FuturePositionX, coll) then
                    for k,x in ipairs(TileMap.Colliders) do
                        TestTile = TileMap.Tiles[(SurroundingTile[1] + (TileMap.Width * SurroundingTile[2]))+1]
                        if TestTile == x then
                            xMovement = 0

                        end
                    end
                end
            end
        end
    end
    
    yMovement = math.clamp(yMovement, Attributes.MaxYSpeed * -1, Attributes.MaxYSpeed )
    
    Self.GetTransform(Self).Transform.Pos.x = Self.GetTransform(Self).Transform.Pos.x + xMovement
    if not boxcoll then
        Self.GetTransform(Self).Transform.Pos.y = (Self.GetTransform(Self).Transform.Pos.y + yMovement)
    end
   -- Self.GetMoveComponent(Self).Vector.x = xMovement
  --  Self.GetMoveComponent(Self).Vector.y = yMovement
    if Self.GetTransform(Self).Transform.Pos.y >= Attributes.KillHeight then
        KillPlayer()
    end
    
end

function OnDestroy()
    print ("Destroyed!")
end