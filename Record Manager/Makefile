clean:
	rm recordMgr
	rm recordMgrExprTest

test: 
	gcc -o recordMgr test_assign3_1.c record_mgr.c -lm buffer_mgr.c buffer_mgr_stat.c storage_mgr.c dberror.c rm_serializer.c expr.c
	./recordMgr

test_expr:
	gcc -o recordMgrExprTest test_assign3_1.c record_mgr.c -lm buffer_mgr.c buffer_mgr_stat.c storage_mgr.c dberror.c rm_serializer.c expr.c
	./recordMgrExprTest

