import json
from subprocess import run

def llm_api(prompt): pass

def check_json(text): pass

def LLM(prompt):
    llm_answer = llm_api(prompt)
    while not check_json(llm_answer):
        llm_answer = llm_api("ANSWER SHOULD BE JSON FORMATTED! " + prompt)
    thought = llm_answer["thought"]
    action = llm_answer["action"]
    return thought, action


def run_action(action):
    '''
    action = '{"tool": "read_file", "args": {"path": "main.py"}}'
    '''
    data = json.loads(action)

    action_tool = data["tool"]
    args        = data["args"]

    if action_tool == "read_file":
        result = run("bash", "cat", args["path"])
        return f"Result of running {action_tool} with args {args}: {result}"

    if action_tool == "finish":
        return f"Done"

    return f"Unknown action: {action_tool}"


def build_prompt(task, history):
    return f"""
        You are a coding agent.
        Current task: {task}
        History: {history}
        Think about the next step and choose one action.
        Return structured json output in the following format:
        {
            "thought": "<your thouts based on the task and the history>",
            "action": "<action agent should run to solve the task>"
        }
        """


def agent_loop(task: str):
    history = []

    while True:
        prompt = build_prompt(task, history)
        thought, action = LLM(prompt)

        if action == "finish":
            break

        observation = run_action(action)
        print("OBSERVATION:", observation)

        history.append(f"Thought: {thought}")
        history.append(f"Action: {action}")
        history.append(f"Observation: {observation}")