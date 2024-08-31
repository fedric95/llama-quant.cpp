import sys
import json
import random

# Usage: python tojson.py <input_file> <output_file>


# Adds a question with the correct answer index shuffled
def addQuestion(data, question, answers, labels):
    indices = list(range(len(answers)))
    random.shuffle(indices)

    shuffled_answers = [answers[i] for i in indices]
    shuffled_labels = [labels[i] for i in indices]

    data.append({
        "multiple_correct": {"answers": [], "labels": []},
        "question": f"Question: \"{question}\" Answer:",
        "single_correct": {"answers": shuffled_answers, "labels": shuffled_labels}
    })

# Adds a question once for each possible answer
# shuffles so that the correct index is used once for possible each position
def addMultipleQuestions(data, question, answers, labels):
    for i in range(len(answers)):
        correct_answer_index = labels.index(1)
        shuffled_indices = list(range(len(answers)))
        shuffled_indices.remove(correct_answer_index)
        random.shuffle(shuffled_indices)
        shuffled_indices.insert(i, correct_answer_index)

        shuffled_answers = [answers[j] for j in shuffled_indices]
        shuffled_labels = [labels[j] for j in shuffled_indices]

        data.append({
            "multiple_correct": {"answers": [], "labels": []},
            "question": f"Question: \"{question}\" Answer:",
            "single_correct": {"answers": shuffled_answers, "labels": shuffled_labels}
        })

# Expects the correct answer to be A1
def convert_to_json(file):
    with open(file, 'r', encoding="utf-8") as f:
        question = ""
        answers = []
        labels = []
        data = []
        for line in f:
            line = line.strip()
            if not line:

                addQuestion(data, question, answers, labels)
                question = ""
                answers = []
                labels = []
            elif line.startswith("Q:] "):
                question = line[4:].strip()
            elif line.startswith("A1:] "):
                answers.append(line[5:].strip())
                labels.append(1)
            else:
                answers.append(line[5:].strip())
                labels.append(0)

        # Pick up the last one
        addQuestion(data, question, answers, labels)
        return json.dumps(data, indent=4, ensure_ascii=False)

def write_file(filename, data):
    with open(filename, 'w') as f:
        f.write(data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python tojson.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    json_data = convert_to_json(input_file)
    write_file(output_file, json_data)