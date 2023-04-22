NEWLINE_TRANS = str.maketrans('>', '\n')

def pairwise(iterable):
    "s -> (s0, s1), (s2, s3), (s4, s5), ..."
    a = iter(iterable)
    return zip(a, a)

def convert_newlines(text):
	return text.translate(NEWLINE_TRANS)

def choice(question_id, next_id, tag):
	return [
		f'(answer',
		f'  (question-id {question_id})',
		f'  (tag {tag})',
		f'  (next-id {next_id}))'
	]

def page(page_id, displayText, *raw_choices):
	answers = dict(zip("abcde", pairwise(raw_choices)))

	displayAnswers = '\n'.join(
		f'{tag}: {convert_newlines(text)}' for tag, (_, text) in answers.items()
	)
	validAnswers = ' '.join(a for a in answers)

	return '\n'.join([
		f'(question',
		f'  (id {page_id})',
		f'  (the-question "{convert_newlines(displayText)}\n\n{displayAnswers}")',
		f'  (valid-answers {validAnswers}))',
	] + [c for tag, (next_id, _) in answers.items() for c in choice(page_id, next_id, tag)])

with open('hobbit.tsv') as inp:
	rows = inp.read().strip().split('\n')[3::]
with open('hobbit.q.clp', 'w') as out:
	print('(deffacts HOBBIT-QUESTIONS::question-attributes', *(page(*line.strip().split('\t')) for line in rows), ')', sep='\n', file=out)