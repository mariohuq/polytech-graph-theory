; RUN:
; clips -f2 hobbit.clp

(defmodule MAIN (export ?ALL))

; functions

; start snippet ask-question
(deffunction MAIN::ask-question (?question ?allowed-answers)
  (bind ?answer "")
  (while (not (member ?answer ?allowed-answers)) do
    (printout t ?question crlf ">")
    (bind ?answer (read))
    (if (lexemep ?answer)
      then (bind ?answer (lowcase ?answer))))
  ?answer)
; end snippet ask-question
(deftemplate MAIN::end-of-story)

(defmodule QUESTIONS (import MAIN ?ALL) (export ?ALL))

; start snippet deftemplate
(deftemplate QUESTIONS::question
   (slot id)
   (slot the-question)
   (multislot valid-answers))

(deftemplate QUESTIONS::response
   (slot question-id)
   (slot tag))

(deftemplate QUESTIONS::answer
   (slot question-id)
   (slot tag)
   (slot next-id))
; end snippet deftemplate

; start snippet ask-a-question
(defrule QUESTIONS::ask-a-question
  (question
     (id ?question-id)
     (the-question ?the-question)
     (valid-answers $?valid-answers))
  (answer
      (question-id ?prev-question-id)
      (next-id ?question-id-1)
      (tag ?tag))
  (response
      (question-id ?prev-question-id-1)
      (tag ?tag-1))
  (test (and
    (= ?question-id ?question-id-1)
    (= ?prev-question-id ?prev-question-id-1)
    (eq ?tag ?tag-1)))
  =>
  (if (= (length ?valid-answers) 0)
    then
      (assert (end-of-story))
      (printout t ?the-question)
    else (assert (response
      (question-id ?question-id)
      (tag (ask-question ?the-question ?valid-answers))))))
; end snippet ask-a-question

(defmodule HOBBIT-QUESTIONS (import QUESTIONS ?ALL) (export ?ALL))

; start snippet question-init
(deffacts HOBBIT-QUESTIONS::question-init
  (response (question-id 0) (tag a))
  (answer (question-id 0) (next-id 1) (tag a)))
; end snippet question-init

; start snippet load
(load* "hobbit.q.clp")
; end snippet load

; STARTUP
(defrule MAIN::system-banner
  (declare (salience 2000)) =>
    (printout t crlf)
    (printout t "Хоббит: Путешествие")
    (printout t crlf crlf))

(defrule MAIN::start
  (declare (salience 1000))
  =>
  (focus QUESTIONS HOBBIT-QUESTIONS))

(defrule MAIN::game-over
  (end-of-story) =>
  (printout t "Игра окончена." crlf))

(reset)
(run)
(exit)
