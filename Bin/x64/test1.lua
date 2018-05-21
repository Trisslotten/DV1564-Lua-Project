for i=1,10 do addBox({i,i,i},1.0-i/11.0) end 
for k,v in pairs(getNodes()) do for kk,vv in pairs(v) do print(k,kk,vv) end end 
camera({-10,0,0},{0,2,0}) 
snapshot("test1.png")