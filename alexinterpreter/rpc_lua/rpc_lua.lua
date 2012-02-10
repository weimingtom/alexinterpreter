

function func(a, b, c)
	return a, b, b+c.a
end


print(rpc_call("func", 1, 2, {a=11, b=22}))




function dump(tb)
	table.foreach(tb,
						function(i, v)
							if(type(v)=='table') then
								print(i)
								dump(v)
							else
								print (i, v)
							end
						end
				)
end

