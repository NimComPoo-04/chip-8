(define src 
  "cls
   ret
   = fucker 10
   ;; hello bitch
   :lable
   jmp 100
   add v0 100")

;;
;;      retrives a single line from the buffer
;;
(define get-line (lambda (l)
    (cond
      ((equal? l '()) '())
      ((equal? (car l) #\newline) '())
      (else (cons (car l) (get-line (cdr l)))))))

;;
;;      Delimiters: ' ' '\t' and '\n' -- IDK why i use the c notation but whatever
;;

(define get-word
  (lambda (l)
    (cond
      ((equal? l '()) '())
      ((equal? (car l) #\space) '())
      ((equal? (car l) #\tab) '())
      ((equal? (car l) #\newline) '())
      ((equal? (car l) #\;) (get-line l))
      ((equal? (car l) #\:) (list (car l)))
      ((equal? (car l) #\=) (list (car l)))
      (else (cons (car l) (get-word (cdr l)))))))

;;
;;      Splits the list of chars into separate word list
;;      TODO: comments, lables and constants
;;

(define get-words
  (lambda (l)
    (cond
      ((equal? l '()) '())
      (else (let ((wrd (get-word l)))
	      (cond
		((equal? wrd '()) (get-words (list-tail l 1)))
		(else (cons wrd
			    (get-words (list-tail l (length wrd)))))))))))
;;
;;      Classify -- classifies the opcodes and other recognizable tokens
;;      into a favorable structure which might be easier to process later
;;      during the parsing phase
;;

(define classify
  (lambda (l)
    (cond
      ((equal? l "") '())

      ((or (equal? l "cls") (equal? l "ret")) (list 'opcode 0 l))

      ((or (equal? l "jmp") (equal? l "jsr") (equal? l "jrl")
	   (equal? l "bcd") (equal? l "stm") (equal? l "ldm")
	   (equal? l "sprite")) (list 'opcode 1 l))

      ((or (equal? l "ldr") (equal? l "add") (equal? l "or") (equal? l "and")
	   (equal? l "xor") (equal? l "adc") (equal? l "sub") (equal? l "shl")
	   (equal? l "shr") (equal? l "rnd") (equal? l "key")
	   (equal? l "mov")) (list 'opcode 2 l))

      ((or (equal? l "brc") (equal? l "draw")) (list 'opcode 3 l))

      ((or (equal? l "v0") (equal? l "v1") (equal? l "v2") (equal? l "v3")
	   (equal? l "v4") (equal? l "v5") (equal? l "v6") (equal? l "v7")
	   (equal? l "v8") (equal? l "v9") (equal? l "va") (equal? l "vb")
	   (equal? l "vc") (equal? l "vd") (equal? l "ve") (equal? l "vf")
	   (equal? l "dt") (equal? l "st") (equal? l  "i")) (list 'register 0 l))

      ((string->number l) (list 'number 0 l))

      ((equal? (string-ref l 0) #\;) (list 'comment 0 l))

      ((equal? (string-ref l 0) #\:) (list 'lable 1 l))

      ((equal? (string-ref l 0) #\=) (list 'const 2 l))

      (else (list 'word 0 l)))))

;;
;;      Parser parses the whole list one statement at a time
;;

(define parser
  (lambda (l)
    (cond
      ((equal? l '()) '())
      ((equal? (cadar l) 0) (cons (list (car l))
				  (parser (cdr l))))
      ((equal? (cadar l) 1) (cons (list (car l) (cadr l))
				  (parser (cddr l))))
      ((equal? (cadar l) 2) (cons (list (car l) (cadr l) (caddr l))
				  (parser (cdddr l))))
      ((equal? (cadar l) 3) (cons (list (car l) (cadr l) (caddr l) (cadddr l))
				  (parser (cddddr l)))))))
;;
;;      Remove comments dude those are just weird
;;

(define rem-crust
  (lambda (l)
    (cond
      ((equal? l '()) '())
      ((equal? (caaar l) 'comment) (rem-crust (cdr l)))
      ((equal? (caaar l) 'const) (rem-crust (cdr l)))
      ((equal? (caaar l) 'lable) (rem-crust (cdr l)))
      (else (cons (car l) (rem-crust (cdr l)))))))

;;
;;	Pre process constants and lables
;;

(define preprop
  (lambda (l n)
    (cond
      ((equal? l '()) '())
      ((equal? (caaar l) 'lable) (cons (list (caddr (cadar l)) n)
				       (preprop (cdr l) n)))
      ((equal? (caaar l) 'const) (cons (list (caddr (cadar l))
					     (caddr (caddar l)))
				       (preprop (cdr l) n)))
      ((equal? (caaar l) 'opcode) (preprop (cdr l) (+ n 2)))
      (else (preprop (cdr l) n)))))

;;
;; Compile ast lol
;;

;;
;;	Pretty Printing stuff lol
;;	'cause why tf not
;;

(define pretty
  (lambda (l)
    (cond
      ((equal? l '()) '())
      (else (begin (display (car l))
		   (newline)
		   (pretty (cdr l)))))))

(define ast (parser (map classify
			 (map list->string
			      (get-words (string->list src))))))

(define ast-without-crust (rem-crust ast))
(define crepe-list-lol (preprop ast #x200))

(display "+-------------+\n")
(display "| SOURCE CODE |\n")
(display "+-------------+\n")
(pretty ast-without-crust)
(newline)

(display "+-----------+\n")
(display "| CONSTANTS |\n")
(display "+-----------+\n")
(pretty crepe-list-lol)
(newline)

(exit)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Friendly reminder that this is the slowest crap i have ever excreted out and i am sorry about it :( ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
