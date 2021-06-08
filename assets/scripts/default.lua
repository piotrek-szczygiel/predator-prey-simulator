function update_chicken(chicken)
    chicken.energy = chicken.energy - Config.chicken.energyLoss - chicken.genes.sensorRange

    if chicken:isDead() or chicken.lastMove < 3 then
        return
    end

    if chicken.energy <= Config.chicken.hungerStart then
        chicken.hungry = true
    elseif chicken.energy >= Config.chicken.hungerStop then
        chicken.hungry = false
    end

    local wolf, dist, step = chicken:pathToNearest(Type.Wolf)
    if wolf then
        chicken:resetRandom()
        chicken:moveStep(-step)
        return
    end

    if chicken.hungry then
        local grass, dist, step = chicken:pathToNearest(Type.Grass)
        if grass then
            if dist == 1 then
                grass:kill()
                chicken.energy = chicken.energy + Config.grass.nutritionalValue
            end

            chicken:resetRandom()
            chicken:moveStep(step)
            return
        end
    else
        local partner, dist, step = chicken:pathToNearestFed(Type.Chicken)
        if partner then
            if dist == 1 then
                chicken:breed(partner)
            else
                chicken:moveStep(step)
            end

            chicken:resetRandom()
            return
        end
    end

    chicken:moveRandom(chicken.genes.sensorRange)
end

function update_wolf(wolf)
    wolf.energy = wolf.energy - Config.wolf.energyLoss - wolf.genes.sensorRange

    if wolf:isDead() or wolf.lastMove < 2 then
        return
    end

    if wolf.energy <= Config.wolf.hungerStart then
        wolf.hungry = true
    elseif wolf.energy >= Config.wolf.hungerStop then
        wolf.hungry = false
    end

    if wolf.hungry then
        local chicken, dist, step = wolf:pathToNearest(Type.Chicken)
        if chicken then
            if dist == 1 then
                chicken:kill()
                wolf.energy = wolf.energy + Config.chicken.nutritionalValue
            end

            wolf:resetRandom()
            wolf:moveStep(step)
            return
        end
    else
        local partner, dist, step = wolf:pathToNearestFed(Type.Wolf)
        if partner then
            if dist == 1 then
                wolf:breed(partner)
            else
                wolf:moveStep(step)
            end

            wolf:resetRandom()
            return
        end
    end

    wolf:moveRandomAnywhere()
end
