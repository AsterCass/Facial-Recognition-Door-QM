import sqlite3
import uuid
from datetime import datetime

from flask import Flask, request, jsonify

app = Flask(__name__)

dbAddress = 'task.db'
serverPort = 5525


# DDL
def init_db():
    conn = sqlite3.connect(dbAddress)
    c = conn.cursor()
    c.execute('''
        CREATE TABLE IF NOT EXISTS tasks (
            task_id TEXT PRIMARY KEY,
            action INTEGER,
            user_id TEXT,
            face_photo TEXT,
            device_id TEXT,
            data_type INTEGER,
            status INTEGER,
            start_time INTEGER,
            end_time INTEGER, 
            create_time INTEGER
        )
    ''')
    conn.commit()
    conn.close()


# Init DB
init_db()


@app.route('/api/face/add', methods=['POST'])
def add_face():
    try:
        data = request.json

        # Check param
        required_fields = ['action', 'deviceId', 'endTime', 'startTime', 'facePhoto', 'userId']
        for field in required_fields:
            if field not in data:
                return jsonify({'error': f'Missing required field: {field}'}), 400

        # Generate task id
        task_id = str(uuid.uuid4())

        # Insert
        conn = sqlite3.connect(dbAddress)
        c = conn.cursor()
        c.execute('''
            INSERT INTO tasks (task_id, action, user_id, face_photo, device_id, data_type, status, start_time, end_time, create_time)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            task_id,
            data['action'],
            data['userId'],
            data['facePhoto'],
            data['deviceId'],
            0x1002,
            0,
            data['startTime'],
            data['endTime'],
            int(datetime.now().timestamp())
        ))

        conn.commit()
        conn.close()

        return jsonify({
            'code': 200,
            'message': 'Success',
            'data': {'taskId': task_id}
        })

    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/tasks', methods=['GET'])
def get_tasks():
    try:
        device_id = request.args.get('deviceId')
        if not device_id:
            return jsonify({'error': 'deviceId is required'}), 400

        conn = sqlite3.connect(dbAddress)
        c = conn.cursor()

        # Get tasks
        c.execute('''
            SELECT *
            FROM tasks 
            WHERE device_id = ? AND status = 0
        ''', (device_id,))

        tasks = []
        for row in c.fetchall():
            task = {
                'dataType': row[5],
                'taskData': {
                    'taskId': row[0],
                    'action': row[1],
                    'userId': row[2],
                    'facePhoto': row[3],
                    'deviceId': row[4],
                    'dataType': row[5],
                    'status': row[6],
                    'startTime': row[7],
                    'endTime': row[8],
                    'createTime': row[9],
                }
            }
            tasks.append(task)

            # Update task status
            c.execute('''
                UPDATE tasks 
                SET status = 1 
                WHERE task_id = ?
            ''', (row[0],))

        conn.commit()
        conn.close()

        return jsonify({
            'code': 200,
            'message': 'Success',
            'data': tasks
        })

    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/tasks/done', methods=['GET'])
def complete_task():
    try:
        task_id = request.args.get('taskId')
        if not task_id:
            return jsonify({'error': 'taskId is required'}), 400

        conn = sqlite3.connect(dbAddress)
        c = conn.cursor()

        # Update task status
        c.execute('''
            UPDATE tasks 
            SET status = 2 
            WHERE task_id = ?
        ''', (task_id,))

        if c.rowcount == 0:
            conn.close()
            return jsonify({'error': 'Task not found'}), 404

        conn.commit()
        conn.close()

        return jsonify({
            'code': 200,
            'message': 'Success'
        })

    except Exception as e:
        return jsonify({'error': str(e)}), 500


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=serverPort)
