(defun soucin (arr1 arr2)

  (if (eq arr1 nil)
  	(if (eq arr2 nil)
	    nil
	    (cons (car arr2) (soucin arr1 (cdr arr2)))	
	)

	(if (eq arr2 nil)
	  (cons (car arr1) (soucin (cdr arr1) arr2))
	  
	)
  )
)  
